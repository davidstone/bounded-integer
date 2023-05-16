// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <std_module/prelude.hpp>
#include <benchmark/benchmark.h>

import containers.algorithms.sort.insertion_sort;

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
	constexpr explicit data(unsigned value) {
		for (auto const n : containers::integer_range(bounded::constant<size>)) {
			m[n] = static_cast<std::uint8_t>(value);
		}
	}
	friend constexpr auto operator<=>(data const & lhs, data const & rhs) = default;
	containers::array<std::uint8_t, bounded::constant<size>> m;
};

template<std::size_t data_size>
auto benchmark_impl(benchmark::State & state, auto function) -> void {
	auto engine = std::mt19937(std::random_device()());
	auto value_distribution = std::uniform_int_distribution<unsigned>();
	using container_t = containers::vector<data<data_size>>;
	auto container = container_t();
	using size_type = containers::range_size_t<container_t>;
	auto const size = bounded::assume_in_range<size_type>(state.range(0));
	container.reserve(size);

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

template<std::size_t data_size>
auto benchmark_chunked_insertion_sort(benchmark::State & state) -> void {
	benchmark_impl<data_size>(state, [](auto & range) { return containers::chunked_insertion_sort(range); });
}

template<std::size_t data_size>
auto benchmark_mine(benchmark::State & state) -> void {
	benchmark_impl<data_size>(state, containers::new_sort);
}

template<std::size_t data_size>
auto benchmark_standard(benchmark::State & state) -> void {
	benchmark_impl<data_size>(state, containers::sort);
}

constexpr auto insertion_sort = [](containers::range auto && r) -> void {
	auto const first = containers::begin(r);
	auto const last = containers::end(r);
    for (auto it = first; it != last; ++it) {
        auto const insertion = containers::find_last(containers::range_view(first, it), *it);
		// TODO: Relocate-based algorithm
        std::rotate(
			containers::make_legacy_iterator(insertion),
			containers::make_legacy_iterator(it),
			containers::make_legacy_iterator(containers::next(it))
		);
    }
};

template<std::size_t data_size>
auto benchmark_insertion_sort(benchmark::State & state) -> void {
	benchmark_impl<data_size>(state, insertion_sort);
}

#define BENCHMARK_ALL(data_size) \
	BENCHMARK(benchmark_insertion_sort<data_size>)->DenseRange(1, 5, 1)->Arg(16)->Arg(25)->Arg(64)->Arg(128); \
	BENCHMARK(benchmark_chunked_insertion_sort<data_size>)->DenseRange(1, 5, 1)->Arg(16)->Arg(25)->Arg(64)->Arg(128); \
	BENCHMARK(benchmark_mine<data_size>)->DenseRange(1, 5, 1)->Arg(16)->Arg(25)->Arg(64)->Arg(128); \
	BENCHMARK(benchmark_standard<data_size>)->DenseRange(1, 5, 1)->Arg(16)->Arg(25)->Arg(64)->Arg(128)

BENCHMARK_ALL(1);
BENCHMARK_ALL(4);
BENCHMARK_ALL(8);
BENCHMARK_ALL(64);
BENCHMARK_ALL(65);

} // namespace
