// Copyright David Stone 2019, Malte Skarupke 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/double_buffered_ska_sort.hpp>
#include <containers/algorithms/is_sorted.hpp>
#include <containers/algorithms/ska_sort.hpp>
#include <containers/algorithms/sort.hpp>
#include <containers/array.hpp>
#include <containers/c_array.hpp>
#include <containers/front_back.hpp>
#include <containers/push_back.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/static_vector.hpp>
#include <containers/string.hpp>
#include <containers/vector.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <benchmark/benchmark.h>

#include <random>

namespace {

using namespace containers;

constexpr void inplace_radix_sort(range auto && to_sort, auto extract_key) {
	detail::inplace_radix_sort<1>(
		containers::range_view(containers::begin(to_sort), containers::end(to_sort)),
		extract_key
	);
}

constexpr void inplace_radix_sort(range auto && to_sort) {
	inplace_radix_sort(to_sort, containers::to_radix_sort_key);
}

template<typename Container>
auto create_radix_sort_data(std::mt19937_64 & engine, bounded::bounded_integer auto const size, auto distribution) {
	return Container(containers::generate_n(size, [&] { return distribution(engine); }));
}

auto create_radix_sort_data(std::mt19937_64 & engine, bounded::bounded_integer auto const size, auto distribution) {
	return create_radix_sort_data<containers::vector<decltype(distribution(engine))>>(engine, size, distribution);
}

constexpr int profile_multiplier = 2;
constexpr int max_profile_range = 1 << 20;

auto get_value(benchmark::State const & state) {
	return bounded::assume_in_range(
		bounded::integer(state.range(0)),
		0_bi,
		bounded::constant<numeric_traits::max_value<std::int64_t>>
	);
}

void benchmark_double_buffered_ska_sort(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	auto buffer = create(randomness, get_value(state));
	for (auto _ : state) {
		auto to_sort = create(randomness, get_value(state));
		using containers::data;
		benchmark::DoNotOptimize(data(to_sort));
		benchmark::DoNotOptimize(data(buffer));
#ifdef SORT_ON_FIRST_ONLY
		double_buffered_ska_sort(to_sort, buffer, [](auto && a){
			using std::get;
			return get<0>(a);
		});
#else
		bool which = double_buffered_ska_sort(to_sort, buffer);
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
	create(randomness, get_value(state));
	for (auto _ : state) {
		auto to_sort = create(randomness, get_value(state));
		benchmark::DoNotOptimize(containers::data(to_sort));
#ifdef SORT_ON_FIRST_ONLY
		containers::sort(to_sort, [](auto && l, auto && r){ return std::get<0>(l) < std::get<0>(r); });
#else
		containers::sort(to_sort);
		assert(containers::is_sorted(to_sort));
#endif
		benchmark::ClobberMemory();
	}
}

void american_flag_sort(range auto && to_sort, auto && extract_key) {
	detail::inplace_radix_sort<numeric_traits::max_value<std::ptrdiff_t>>(
		containers::range_view(
			containers::begin(to_sort),
			containers::end(to_sort)
		),
		extract_key
	);
}

void american_flag_sort(range auto && to_sort) {
	american_flag_sort(to_sort, containers::to_radix_sort_key);
}

void benchmark_american_flag_sort(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	create(randomness, get_value(state));
	for (auto _ : state) {
		auto to_sort = create(randomness, get_value(state));
		benchmark::DoNotOptimize(containers::data(to_sort));
#ifdef SORT_ON_FIRST_ONLY
		american_flag_sort(to_sort, [](auto && a) -> decltype(auto){ return std::get<0>(a); });
#else
		american_flag_sort(to_sort);
		assert(containers::is_sorted(to_sort));
#endif
		benchmark::ClobberMemory();
	}
}

void benchmark_ska_sort(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	create(randomness, get_value(state));
	for (auto _ : state) {
		auto to_sort = create(randomness, get_value(state));
		benchmark::DoNotOptimize(containers::data(to_sort));
#ifdef SORT_ON_FIRST_ONLY
		ska_sort(to_sort, [](auto && a) -> decltype(auto){ return std::get<0>(a); });
#else
		ska_sort(to_sort);
		assert(containers::is_sorted(to_sort));
#endif
		benchmark::ClobberMemory();
	}
}

void benchmark_inplace_radix_sort(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	create(randomness, get_value(state));
	for (auto _ : state) {
		auto to_sort = create(randomness, get_value(state));
		benchmark::DoNotOptimize(containers::data(to_sort));
#ifdef SORT_ON_FIRST_ONLY
		inplace_radix_sort(to_sort, [](auto && a) -> decltype(auto) { return std::get<0>(a); });
#else
		inplace_radix_sort(to_sort);
		assert(containers::is_sorted(to_sort));
#endif
		benchmark::ClobberMemory();
	}
}

void benchmark_generation(benchmark::State & state, auto create) {
	auto randomness = std::mt19937_64(77342348);
	create(randomness, get_value(state));
	for (auto _ : state) {
		auto to_sort = create(randomness, get_value(state));
		benchmark::DoNotOptimize(containers::data(to_sort));
		benchmark::ClobberMemory();
	}
}


auto create_limited_radix_sort_data(std::mt19937_64 & randomness, std::int8_t range_end) {
	auto int_distribution = std::uniform_int_distribution<std::int8_t>(-128, range_end);
	return containers::vector(containers::generate_n(
		2_bi * 1024_bi * 1024_bi,
		[&] { return int_distribution(randomness); }
	));
}
void benchmark_limited_generation(benchmark::State & state) {
	auto randomness = std::mt19937_64(77342348);
	for (auto _ : state) {
		auto to_sort = create_limited_radix_sort_data(randomness, static_cast<std::int8_t>(get_value(state)));
		benchmark::DoNotOptimize(containers::data(to_sort));
		benchmark::ClobberMemory();
	}
}

#define LIMITED_RANGE() Arg(-128)->Arg(-127)->Arg(-120)->Arg(-96)->Arg(-64)->Arg(-32)->Arg(0)->Arg(32)->Arg(64)->Arg(96)->Arg(127)
BENCHMARK(benchmark_limited_generation)->LIMITED_RANGE();

void benchmark_limited_inplace_sort(benchmark::State & state) {
	std::mt19937_64 randomness(77342348);
	for (auto _ : state) {
		auto to_sort = create_limited_radix_sort_data(randomness, static_cast<std::int8_t>(get_value(state)));
		benchmark::DoNotOptimize(containers::data(to_sort));
		ska_sort(to_sort);
		assert(containers::is_sorted(to_sort));
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
	return [=](auto & engine, bounded::bounded_integer auto const size) {
		return create_radix_sort_data(engine, size, distribution);
	};
};

// Technically this does not allow sharing a distribution for the same type, but
// that is probably not a serious problem for this use case
auto create_tuple_data = [](auto... distributions) {
	return [=](auto & engine, bounded::bounded_integer auto const size) mutable {
		return create_radix_sort_data(engine, size, [&](auto &) {
			return std::tuple(distributions(engine)...);
		});
	};
};

auto create_range_data = [](bounded::bounded_integer auto const max_size, auto generate) {
	auto size_distribution = std::uniform_int_distribution(0, static_cast<int>(max_size));
	return [=](auto & engine, bounded::bounded_integer auto const size) mutable {
		return create_radix_sort_data(engine, size, [&](auto &) {
			return generate(engine, bounded::assume_in_range(size_distribution(engine), 0_bi, max_size));
		});
	};
};

using numeric_traits::min_value;
using numeric_traits::max_value;

auto make_bool_distribution() {
	return [int_distribution = std::uniform_int_distribution<short>(0, 1)](std::mt19937_64 & engine) mutable {
		return int_distribution(engine) != 0;
	};
}

void register_all_benchmarks() {
	REGISTER_BENCHMARK(
		"bool",
		create_simple_data(make_bool_distribution())
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
			20_bi,
			[value_distribution = std::uniform_int_distribution()](auto & engine, bounded::bounded_integer auto size) mutable {
				return create_radix_sort_data(engine, size, value_distribution);
			}
		)
	);
	REGISTER_SOME_BENCHMARKS(
		"vector_int_wide",
		create_range_data(128_bi, [](auto &, auto size) {
			return containers::vector(containers::integer_range(static_cast<int>(size)));
		})
	);
	REGISTER_SOME_BENCHMARKS(
		"string",
		create_range_data(20_bi, [char_distribution = std::uniform_int_distribution('a', 'z')](auto & engine, auto size) mutable {
			return create_radix_sort_data<containers::string>(engine, size, char_distribution);
		})
	);
	REGISTER_SOME_BENCHMARKS(
		"vector_string",
		create_range_data(10_bi, [](auto & engine, auto size) {
			auto function = [&, char_distribution = std::uniform_int_distribution('a', 'c')](auto &, auto inner_size) mutable {
				return create_radix_sort_data<containers::string>(engine, inner_size, char_distribution);
			};
			return create_range_data(5_bi, function)(engine, size);
		})
	);
}

} // namespace

int main(int argc, char * * argv) {
	register_all_benchmarks();
	benchmark::Initialize(&argc, argv);
	benchmark::RunSpecifiedBenchmarks();
}