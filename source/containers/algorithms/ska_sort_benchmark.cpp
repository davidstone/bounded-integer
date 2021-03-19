// Copyright David Stone 2019, Malte Skarupke 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/ska_sort.hpp>
#include <containers/algorithms/sort.hpp>
#include <containers/array/array.hpp>
#include <containers/array/make_array.hpp>
#include <containers/c_array.hpp>
#include <containers/front_back.hpp>
#include <containers/push_back.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/static_vector/static_vector.hpp>
#include <containers/string.hpp>
#include <containers/vector.hpp>

#include <bounded/detail/tuple.hpp>
#include <bounded/copy.hpp>

#include <benchmark/benchmark.h>

#include <deque>
#include <random>
#include <vector>

#include "../../test_assert.hpp"

namespace {

using namespace containers;

constexpr void inplace_radix_sort(auto const first, auto const last, auto extract_key) {
	detail::inplace_radix_sort<1>(first, last, extract_key);
}

constexpr void inplace_radix_sort(auto const first, auto const last) {
	inplace_radix_sort(first, last, bounded::identity);
}

#define SKA_SORT_NOINLINE __attribute__((noinline))

template<typename Container>
auto create_radix_sort_data(std::mt19937_64 & engine, std::int64_t const size, auto distribution) {
	auto result = Container();
	containers::detail::reserve_if_reservable(result, static_cast<typename Container::size_type>(size));
	for (std::int64_t n = 0; n != size; ++n) {
		containers::push_back(result, distribution(engine));
	}
	return result;
}

auto create_radix_sort_data(std::mt19937_64 & engine, std::int64_t const size, auto distribution) {
	return create_radix_sort_data<std::vector<decltype(distribution(engine))>>(engine, size, distribution);
}


#if 0

extern const std::vector<const char *> & get_word_list();
std::vector<std::string> SKA_SORT_NOINLINE create_radix_sort_data(std::mt19937_64 & randomness, std::int64_t size)
{
	const std::vector<const char *> & words = get_word_list();
	std::vector<std::string> result;
	result.reserve(size);
	std::uniform_int_distribution<int> string_length_distribution(0, 10);
	//std::uniform_int_distribution<int> string_length_distribution(1, 3);
	std::uniform_int_distribution<size_t> word_picker(0, words.size() - 1);
	for (std::int64_t outer = 0; outer < size; ++outer)
	{
		std::string to_add;
		for (int inner = 0, end = string_length_distribution(randomness); inner < end; ++inner)
		{
			to_add += words[word_picker(randomness)];
		}
		result.push_back(std::move(to_add));
	}
	return result;
}

#endif

#if 0
struct benchmark_sort_value {
	std::array<uint8_t, 1016> array = {};
};
#define SORT_ON_FIRST_ONLY
typedef std::int64_t benchmark_sort_key;
#define NUM_SORT_KEYS 1
#if NUM_SORT_KEYS == 1
std::vector<std::tuple<benchmark_sort_key, benchmark_sort_value>> SKA_SORT_NOINLINE create_radix_sort_data(std::mt19937_64 & randomness, int size)
{
	std::vector<std::tuple<benchmark_sort_key, benchmark_sort_value>> result;
	result.reserve(size);
	std::uniform_int_distribution<benchmark_sort_key> distribution(bounded::min_value<benchmark_sort_key>, bounded::max_value<benchmark_sort_key>);
	for (int i = 0; i < size; ++i)
	{
		result.push_back(distribution(randomness), benchmark_sort_value());
	}
	return result;
}
#elif NUM_SORT_KEYS == 2
std::vector<std::tuple<std::pair<benchmark_sort_key, benchmark_sort_key>, benchmark_sort_value>> SKA_SORT_NOINLINE create_radix_sort_data(std::mt19937_64 & randomness, int size)
{
	std::vector<std::tuple<std::pair<benchmark_sort_key, benchmark_sort_key>, benchmark_sort_value>> result;
	result.reserve(size);
	std::uniform_int_distribution<benchmark_sort_key> distribution(bounded::min_value<benchmark_sort_key>, bounded::max_value<benchmark_sort_key>);
	for (int i = 0; i < size; ++i)
	{
		result.push_back(std::make_pair(distribution(randomness), distribution(randomness)), benchmark_sort_value());
	}
	return result;
}
#else
std::vector<std::tuple<std::array<benchmark_sort_key, NUM_SORT_KEYS>, benchmark_sort_value>> SKA_SORT_NOINLINE create_radix_sort_data(std::mt19937_64 & randomness, int size)
{
	std::vector<std::tuple<std::array<benchmark_sort_key, NUM_SORT_KEYS>, benchmark_sort_value>> result;
	result.reserve(size);
	std::uniform_int_distribution<benchmark_sort_key> distribution(bounded::min_value<benchmark_sort_key>, bounded::max_value<benchmark_sort_key>);
	for (int i = 0; i < size; ++i)
	{
		std::array<benchmark_sort_key, NUM_SORT_KEYS> key;
		for (int i = 0; i < NUM_SORT_KEYS; ++i)
			key[i] = distribution(randomness);
		result.push_back(key, benchmark_sort_value());
	}
	return result;
}
#endif
#endif

constexpr int profile_multiplier = 2;
constexpr int max_profile_range = 1 << 20;

void benchmark_double_buffered_ska_sort(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	auto buffer = create(randomness, state.range(0));
	for (auto _ : state) {
		auto to_sort = create(randomness, state.range(0));
		using containers::data;
		benchmark::DoNotOptimize(data(to_sort));
		benchmark::DoNotOptimize(data(buffer));
		using containers::begin;
		using containers::end;
#ifdef SORT_ON_FIRST_ONLY
		double_buffered_ska_sort(begin(to_sort), end(to_sort), begin(buffer), [](auto && a){
			using std::get;
			return get<0>(a);
		});
#else
		bool which = double_buffered_ska_sort(begin(to_sort), end(to_sort), begin(buffer));
		if (which)
			assert(containers::is_sorted(buffer));
		else
			assert(containers::is_sorted(to_sort));
#endif
		benchmark::ClobberMemory();
	}
}

void benchmark_std_sort(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	create(randomness, state.range(0));
	for (auto _ : state) {
		auto to_sort = create(randomness, state.range(0));
		benchmark::DoNotOptimize(containers::data(to_sort));
#ifdef SORT_ON_FIRST_ONLY
		std::sort(containers::begin(to_sort), containers::end(to_sort), [](auto && l, auto && r){ return std::get<0>(l) < std::get<0>(r); });
#else
		containers::sort(to_sort);
		assert(std::is_sorted(containers::begin(to_sort), containers::end(to_sort)));
#endif
		benchmark::ClobberMemory();
	}
}

void american_flag_sort(auto const first, auto const last, auto && extract_key) {
	detail::inplace_radix_sort<bounded::max_value<std::ptrdiff_t>>(first, last, extract_key);
}

void american_flag_sort(auto const first, auto const last) {
	american_flag_sort(first, last, bounded::identity);
}

void benchmark_american_flag_sort(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	create(randomness, state.range(0));
	for (auto _ : state) {
		auto to_sort = create(randomness, state.range(0));
		benchmark::DoNotOptimize(containers::data(to_sort));
#ifdef SORT_ON_FIRST_ONLY
		american_flag_sort(containers::begin(to_sort), containers::end(to_sort), [](auto && a) -> decltype(auto){ return std::get<0>(a); });
#else
		american_flag_sort(containers::begin(to_sort), containers::end(to_sort));
		assert(std::is_sorted(containers::begin(to_sort), containers::end(to_sort)));
#endif
		benchmark::ClobberMemory();
	}
}

void benchmark_ska_sort(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	create(randomness, state.range(0));
	for (auto _ : state) {
		auto to_sort = create(randomness, state.range(0));
		benchmark::DoNotOptimize(containers::data(to_sort));
#ifdef SORT_ON_FIRST_ONLY
		ska_sort(containers::begin(to_sort), containers::end(to_sort), [](auto && a) -> decltype(auto){ return std::get<0>(a); });
#else
		ska_sort(containers::begin(to_sort), containers::end(to_sort));
		assert(std::is_sorted(containers::begin(to_sort), containers::end(to_sort)));
#endif
		benchmark::ClobberMemory();
	}
}

void benchmark_inplace_radix_sort(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	create(randomness, state.range(0));
	for (auto _ : state) {
		auto to_sort = create(randomness, state.range(0));
		benchmark::DoNotOptimize(containers::data(to_sort));
#ifdef SORT_ON_FIRST_ONLY
		inplace_radix_sort(containers::begin(to_sort), containers::end(to_sort), [](auto && a) -> decltype(auto) { return std::get<0>(a); });
#else
		inplace_radix_sort(containers::begin(to_sort), containers::end(to_sort));
		assert(std::is_sorted(containers::begin(to_sort), containers::end(to_sort)));
#endif
		benchmark::ClobberMemory();
	}
}

void benchmark_generation(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	create(randomness, state.range(0));
	for (auto _ : state) {
		auto to_sort = create(randomness, state.range(0));
		benchmark::DoNotOptimize(containers::data(to_sort));
		benchmark::ClobberMemory();
	}
}


std::vector<std::int8_t> SKA_SORT_NOINLINE create_limited_radix_sort_data(std::mt19937_64 & randomness, std::int8_t range_end) {
	std::int8_t permutation[256];
	std::iota(permutation, permutation + 256, -128);
	std::shuffle(permutation, permutation + 256, randomness);
	std::vector<std::int8_t> result;
	std::size_t size = 2 * 1024 * 1024;
	result.reserve(size);
	std::uniform_int_distribution<std::int8_t> int_distribution(-128, range_end);
	for (size_t i = 0; i < size; ++i) {
		result.push_back(permutation[to_radix_sort_key(int_distribution(randomness))]);
	}
	return result;
}
void benchmark_limited_generation(benchmark::State & state) {
	auto randomness = std::mt19937_64(77342348);
	for (auto _ : state) {
		auto to_sort = create_limited_radix_sort_data(randomness, state.range(0));
		benchmark::DoNotOptimize(containers::data(to_sort));
		benchmark::ClobberMemory();
	}
}

#define LIMITED_RANGE() Arg(-128)->Arg(-127)->Arg(-120)->Arg(-96)->Arg(-64)->Arg(-32)->Arg(0)->Arg(32)->Arg(64)->Arg(96)->Arg(127)
BENCHMARK(benchmark_limited_generation)->LIMITED_RANGE();

void benchmark_limited_inplace_sort(benchmark::State & state) {
	std::mt19937_64 randomness(77342348);
	for (auto _ : state) {
		auto to_sort = create_limited_radix_sort_data(randomness, state.range(0));
		benchmark::DoNotOptimize(containers::data(to_sort));
		ska_sort(containers::begin(to_sort), containers::end(to_sort));
		assert(std::is_sorted(containers::begin(to_sort), containers::end(to_sort)));
		benchmark::ClobberMemory();
	}
}
BENCHMARK(benchmark_limited_inplace_sort)->LIMITED_RANGE();

#define REGISTER_INDIVIDUAL_BENCHMARK(name, runner, create) \
	do { \
		benchmark::RegisterBenchmark( \
			name, \
			[](auto & state) { runner(state, create); } \
		)->RangeMultiplier(profile_multiplier)->Range(profile_multiplier, max_profile_range); \
	} while (false)

#define REGISTER_SOME_BENCHMARKS(name, create) \
	do { \
		REGISTER_INDIVIDUAL_BENCHMARK("generation_" name, benchmark_generation, create); \
		REGISTER_INDIVIDUAL_BENCHMARK("american_flag_sort_" name, benchmark_american_flag_sort, create); \
		REGISTER_INDIVIDUAL_BENCHMARK("inplace_radix_sort_" name, benchmark_inplace_radix_sort, create); \
		REGISTER_INDIVIDUAL_BENCHMARK("ska_sort_" name, benchmark_ska_sort, create); \
		REGISTER_INDIVIDUAL_BENCHMARK("std_sort_" name, benchmark_std_sort, create); \
	} while (false)

#define REGISTER_BENCHMARK(name, create) \
	do { \
		REGISTER_SOME_BENCHMARKS(name, create); \
		REGISTER_INDIVIDUAL_BENCHMARK("double_buffered_ska_sort_" name, benchmark_double_buffered_ska_sort, create); \
	} while (false)

auto create_simple_data = [](auto distribution) {
	return [=](auto & engine, std::int64_t const size) {
		return create_radix_sort_data(engine, size, distribution);
	};
};

// Technically this does not allow sharing a distribution for the same type, but
// that is probably not a serious problem for this use case
auto create_tuple_data = [](auto... distributions) {
	return [=](auto & engine, std::int64_t const size) mutable {
		return create_radix_sort_data(engine, size, [&](auto &) {
			return std::tuple(distributions(engine)...);
		});
	};
};

auto create_range_data = [](int max_size, auto generate) {
	auto size_distribution = std::uniform_int_distribution(0, max_size);
	return [=](auto & engine, std::int64_t const size) mutable {
		// TODO: Use integer range
		return create_radix_sort_data(engine, size, [&](auto &) {
			return generate(engine, size_distribution(engine));
		});
	};
};

using bounded::min_value;
using bounded::max_value;

auto SKA_SORT_NOINLINE create_radix_sort_data_bool(std::mt19937_64 & engine, std::int64_t const size) {
	auto int_distribution = std::uniform_int_distribution<int>(0, 1);
	return create_radix_sort_data<containers::vector<bool>>(engine, size, [&](auto &) {
		return int_distribution(engine) != 0;
	});
}

void register_all_benchmarks() {
	REGISTER_BENCHMARK(
		"bool",
		create_radix_sort_data_bool
	);
	REGISTER_BENCHMARK(
		"uint8",
		create_simple_data(std::uniform_int_distribution(min_value<std::uint8_t>, max_value<std::uint8_t>))
	);
	REGISTER_BENCHMARK(
		"int8",
		create_simple_data(std::uniform_int_distribution(min_value<std::int8_t>, max_value<std::int8_t>))
	);
	REGISTER_BENCHMARK(
		"uint16",
		create_simple_data(std::uniform_int_distribution(min_value<std::uint16_t>, max_value<std::uint16_t>))
	);
	REGISTER_BENCHMARK(
		"int16",
		create_simple_data(std::uniform_int_distribution(min_value<std::int16_t>, max_value<std::int16_t>))
	);
	REGISTER_BENCHMARK(
		"uint32",
		create_simple_data(std::uniform_int_distribution(min_value<std::uint32_t>, max_value<std::uint32_t>))
	);
	REGISTER_BENCHMARK(
		"int32",
		create_simple_data(std::uniform_int_distribution(min_value<std::int32_t>, max_value<std::int32_t>))
	);
	REGISTER_BENCHMARK(
		"uint64",
		create_simple_data(std::uniform_int_distribution(min_value<std::uint64_t>, max_value<std::uint64_t>))
	);
	REGISTER_BENCHMARK(
		"int64",
		create_simple_data(std::uniform_int_distribution(min_value<std::int64_t>, max_value<std::int64_t>))
	);
	REGISTER_BENCHMARK(
		"tuple_uint8_uint8_uint8_uint8",
		create_simple_data(
			[
				distribution = std::uniform_int_distribution(min_value<std::uint32_t>, max_value<std::uint32_t>)
			](auto & engine) mutable {
				auto result = distribution(engine);
				constexpr auto bits = std::numeric_limits<std::uint8_t>::digits;
				return std::array{
					static_cast<std::uint8_t>(result >> (bits * 0U)),
					static_cast<std::uint8_t>(result >> (bits * 1U)),
					static_cast<std::uint8_t>(result >> (bits * 2U)),
					static_cast<std::uint8_t>(result >> (bits * 3U)),
				};
			}
		)
	);
	REGISTER_BENCHMARK(
		"array_uint8_4",
		create_simple_data(
			[
				distribution = std::uniform_int_distribution(min_value<std::uint32_t>, max_value<std::uint32_t>)
			](auto & engine) mutable {
				auto result = distribution(engine);
				constexpr auto bits = std::numeric_limits<std::uint8_t>::digits;
				return std::array{
					static_cast<std::uint8_t>(result >> (bits * 0U)),
					static_cast<std::uint8_t>(result >> (bits * 1U)),
					static_cast<std::uint8_t>(result >> (bits * 2U)),
					static_cast<std::uint8_t>(result >> (bits * 3U)),
				};
			}
		)
	);

	REGISTER_BENCHMARK(
		"int8_0_1",
		create_simple_data(std::uniform_int_distribution<std::int8_t>(0, 1))
	);
	REGISTER_BENCHMARK(
		"geometric_int8",
		create_simple_data(std::geometric_distribution<std::uint8_t>(0.05))
	);
	REGISTER_BENCHMARK(
		"int64_int64_positive",
		create_tuple_data(
			std::uniform_int_distribution<std::int64_t>(),
			std::uniform_int_distribution<std::int64_t>()
		)
	);
	REGISTER_BENCHMARK(
		"int32_int32_int64",
		create_tuple_data(
			std::uniform_int_distribution(min_value<std::int32_t>, max_value<std::int32_t>),
			std::uniform_int_distribution(min_value<std::int32_t>, max_value<std::int32_t>),
			std::uniform_int_distribution(min_value<std::int64_t>, max_value<std::int64_t>)
		)
	);
	REGISTER_BENCHMARK(
		"bool_float",
		create_tuple_data(
			std::uniform_int_distribution<int>(0, 1),
			std::uniform_real_distribution<float>()
		)
	);
	REGISTER_SOME_BENCHMARKS(
		"vector_int_narrow",
		create_range_data(
			20,
			[value_distribution = std::uniform_int_distribution()](auto & engine, auto size) mutable {
				return create_radix_sort_data(engine, size, value_distribution);
			}
		)
	);
	REGISTER_SOME_BENCHMARKS(
		"vector_int_wide",
		create_range_data(128, [](auto &, auto size) {
			auto to_add = std::vector<int>(size);
			std::iota(to_add.begin(), to_add.end(), 0);
			return to_add;
		})
	);
	REGISTER_SOME_BENCHMARKS(
		"string",
		create_range_data(20, [char_distribution = std::uniform_int_distribution('a', 'z')](auto & engine, auto size) mutable {
			return create_radix_sort_data<std::string>(engine, size, char_distribution);
		})
	);
	REGISTER_SOME_BENCHMARKS(
		"vector_string",
		create_range_data(10, [](auto & engine, auto size) {
			auto function = [&, char_distribution = std::uniform_int_distribution('a', 'c')](auto &, auto inner_size) mutable {
				return create_radix_sort_data<std::string>(engine, inner_size, char_distribution);
			};
			return create_range_data(5, function)(engine, size);
		})
	);
}

} // namespace

int main(int argc, char * * argv) {
	register_all_benchmarks();
	benchmark::Initialize(&argc, argv);
	benchmark::RunSpecifiedBenchmarks();
}