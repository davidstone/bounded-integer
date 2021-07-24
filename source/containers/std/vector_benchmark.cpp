// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <benchmark/benchmark.h>

#include <containers/std/vector.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <iostream>
#include <vector>

namespace {

template<typename Container>
void benchmark_push_back(benchmark::State& state) {
	for (auto _ : state) {
		auto v = Container();
		v.push_back(42);
		benchmark::DoNotOptimize(v.data());
		benchmark::ClobberMemory();
	}
}

template<typename Container>
void benchmark_resize(benchmark::State& state) {
	for (auto _ : state) {
		auto v = Container();
		benchmark::DoNotOptimize(v.data());
		benchmark::ClobberMemory();
		v.resize(static_cast<std::size_t>(state.range(0)));
		benchmark::DoNotOptimize(v.data());
		benchmark::ClobberMemory();
	}
}

BENCHMARK_TEMPLATE(benchmark_push_back, std::vector<int>);
BENCHMARK_TEMPLATE(benchmark_push_back, std_containers::vector<int>);

constexpr auto range_max = 10000;
BENCHMARK_TEMPLATE(benchmark_resize, std::vector<int>)->Range(0, range_max);
BENCHMARK_TEMPLATE(benchmark_resize, std_containers::vector<int>)->Range(0, range_max);

} // namespace

int main(int argc, char** argv) {
	::benchmark::Initialize(&argc, argv);
	if (::benchmark::ReportUnrecognizedArguments(argc, argv)) {
		return 1;
	}
	::benchmark::RunSpecifiedBenchmarks();
}