// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <std_module/prelude.hpp>
#include <benchmark/benchmark.h>

import containers.algorithms.sort.chunked_insertion_sort;

import bounded;
import containers;
import std_module;

namespace {

using namespace bounded::literal;

// https://github.com/google/benchmark/issues/1584
auto DoNotOptimize(auto && value) -> void {
	benchmark::DoNotOptimize(value);
}



template<std::size_t size>
struct data {
	constexpr explicit data(unsigned value):
		m(std::bit_cast<array_t>(value))
	{
	}
	friend constexpr auto operator<=>(data const & lhs, data const & rhs) = default;
	using array_t = containers::array<std::uint8_t, bounded::constant<size>>;
	array_t m;
};

template<std::size_t data_size, auto function>
auto benchmark_impl(benchmark::State & state) -> void {
	auto engine = std::mt19937(std::random_device()());
	auto value_distribution = std::uniform_int_distribution<unsigned>();
	using container_t = containers::vector<data<data_size>>;
	using size_type = containers::range_size_t<container_t>;
	auto const size = bounded::assume_in_range<size_type>(state.range(0));
	auto container = container_t();
	containers::resize(container, size);

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
		benchmark::DoNotOptimize(container);
		benchmark::ClobberMemory();
	}
}

#if 0
template<std::size_t data_size>
auto benchmark_chunked_insertion_sort(benchmark::State & state) -> void {
	benchmark_impl<data_size>(state, containers::chunked_insertion_sort);
}

template<std::size_t data_size>
auto benchmark_mine(benchmark::State & state) -> void {
	benchmark_impl<data_size>(state, containers::new_sort);
}

template<std::size_t data_size>
auto benchmark_standard(benchmark::State & state) -> void {
	benchmark_impl<data_size>(state, containers::sort);
}

#endif

constexpr auto insertion_sort = [](containers::range auto && r) -> void {
	auto const first = containers::begin(r);
	auto const last = containers::end(r);
    for (auto it = first; it != last; ++it) {
        auto const insertion = containers::find_last(containers::subrange(first, it), *it);
		// TODO: Relocate-based algorithm
        std::rotate(
			containers::maybe_legacy_iterator(insertion),
			containers::maybe_legacy_iterator(it),
			containers::maybe_legacy_iterator(containers::next(it))
		);
    }
};

#if 0

template<std::size_t data_size>
auto benchmark_insertion_sort(benchmark::State & state) -> void {
	benchmark_impl<data_size>(state, insertion_sort);
}

#endif

using containers::chunked_insertion_sort;
using containers::new_sort;
using containers::sort;
#define BENCHMARK_ALL(data_size) \
	BENCHMARK(benchmark_impl<data_size, insertion_sort>)->DenseRange(1, 5, 1)->Arg(16)->Arg(25)->Arg(64)->Arg(128); \
	BENCHMARK(benchmark_impl<data_size, chunked_insertion_sort>)->DenseRange(1, 5, 1)->Arg(16)->Arg(25)->Arg(64)->Arg(128); \
	BENCHMARK(benchmark_impl<data_size, new_sort>)->DenseRange(1, 5, 1)->Arg(16)->Arg(25)->Arg(64)->Arg(128); \
	BENCHMARK(benchmark_impl<data_size, sort>)->DenseRange(1, 5, 1)->Arg(16)->Arg(25)->Arg(64)->Arg(128)

BENCHMARK_ALL(1);
BENCHMARK_ALL(4);
BENCHMARK_ALL(8);
BENCHMARK_ALL(64);
BENCHMARK_ALL(65);

} // namespace
