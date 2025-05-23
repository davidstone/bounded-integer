// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <benchmark/benchmark.h>
#include <bounded/assert.hpp>

import containers.algorithms.sort.chunked_insertion_sort;

import bounded;
import containers;
import std_module;
import tv;

namespace {

using namespace bounded::literal;

template<std::size_t size>
struct data {
	data() = default;
	constexpr explicit data(unsigned value) {
		for (auto const n : containers::integer_range(bounded::constant<size>)) {
			m[n] = static_cast<std::uint8_t>(value);
		}
	}
	friend constexpr auto operator<=>(data const & lhs, data const & rhs) = default;

	containers::array<std::uint8_t, bounded::constant<size>> m = {};
};

template<std::size_t data_size, auto function>
auto benchmark_sort(benchmark::State & state) -> void {
	auto engine = std::mt19937(std::random_device()());
	auto value_distribution = std::uniform_int_distribution<unsigned>();
	using container_t = containers::vector<data<data_size>>;
	using size_type = containers::range_size_t<container_t>;
	auto const size = bounded::assume_in_range<size_type>(state.range(0));
	auto container = container_t(containers::repeat_default_n<data<data_size>>(size));

	for (auto _ : state) {
		containers::copy(
			containers::generate_n(
				size,
				[&] { return data<data_size>(value_distribution(engine)); }
			),
			containers::begin(container)
		);
		benchmark::DoNotOptimize(container);
		benchmark::ClobberMemory();
		function(container);
		BOUNDED_ASSERT(containers::is_sorted(container));
		benchmark::DoNotOptimize(container);
		benchmark::ClobberMemory();
	}
}


constexpr auto insertion_sort = [](containers::range auto && r) -> void {
	auto const first = containers::begin(r);
	auto const last = containers::end(r);
    for (auto it = first; it != last; ++it) {
		auto const sorted = containers::subrange(first, it);
		auto const insertion = ::containers::find_if(
			sorted,
			[&](auto const & value) { return *it < value; }
		);
		auto temp = tv::relocate_into_storage(*it);
		containers::uninitialized_relocate_backward(
			containers::subrange(insertion, it),
			containers::next(it)
		);
		bounded::relocate_at(*insertion, temp.value);
    }
};

using containers::chunked_insertion_sort;
using containers::new_sort;
using containers::sort;

#define BENCHMARK_SIZES DenseRange(1, 5, 1)->Arg(16)->Arg(25)->Arg(64)->Arg(128)->Arg(1'000'000)

#define BENCHMARK_ALL(data_size) \
	BENCHMARK(benchmark_sort<data_size, insertion_sort>)->BENCHMARK_SIZES; \
	BENCHMARK(benchmark_sort<data_size, chunked_insertion_sort>)->BENCHMARK_SIZES; \
	BENCHMARK(benchmark_sort<data_size, new_sort>)->BENCHMARK_SIZES; \
	BENCHMARK(benchmark_sort<data_size, sort>)->BENCHMARK_SIZES

BENCHMARK_ALL(1);
BENCHMARK_ALL(4);
BENCHMARK_ALL(8);
BENCHMARK_ALL(64);
BENCHMARK_ALL(65);

} // namespace