// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <benchmark/benchmark.h>

import containers;
import std_module;

namespace {

template<typename T>
auto default_value() {
	if constexpr (std::integral<T>) {
		return 0;
	} else {
		return T(1024, typename T::value_type());
	}
}

template <class Container>
void benchmark_construct_size(benchmark::State & state) {
	auto const size = static_cast<containers::range_size_t<Container>>(state.range(0));
	for (auto _ : state) {
		auto c = Container(size);
		benchmark::DoNotOptimize(c.data());
	}
}

template <class Container, typename Container::value_type value>
void benchmark_construct_repeat_value(benchmark::State & state) {
	auto const size = static_cast<containers::range_size_t<Container>>(state.range(0));
	for (auto _ : state) {
		auto c = Container(size, value);
		benchmark::DoNotOptimize(c.data());
	}
}

template <class Container>
void benchmark_construct_iterator_pair(benchmark::State & state) {
	auto const size = static_cast<containers::range_size_t<Container>>(state.range(0));
	auto in = std::vector(size, default_value<containers::range_value_t<Container>>());
	auto const begin = in.begin();
	auto const end = in.end();
	benchmark::DoNotOptimize(std::addressof(in));
	benchmark::ClobberMemory();
	for (auto _ : state) {
		auto c = Container(begin, end);
		benchmark::DoNotOptimize(c.data());
		benchmark::ClobberMemory();
	}
}

constexpr std::size_t max_iter_range = 1 << 20;

BENCHMARK_TEMPLATE(benchmark_construct_size, std::vector<unsigned char>)->Arg(5140480);
BENCHMARK_TEMPLATE(benchmark_construct_size, std_containers::vector<unsigned char>)->Arg(5140480);
BENCHMARK_TEMPLATE(benchmark_construct_repeat_value, std::vector<unsigned char>, 0)->Arg(5140480);
BENCHMARK_TEMPLATE(benchmark_construct_repeat_value, std_containers::vector<unsigned char>, 0)->Arg(5140480);
BENCHMARK_TEMPLATE(benchmark_construct_iterator_pair, std::vector<char>)->Range(0, max_iter_range);
BENCHMARK_TEMPLATE(benchmark_construct_iterator_pair, std_containers::vector<char>)->Range(0, max_iter_range);
BENCHMARK_TEMPLATE(benchmark_construct_iterator_pair, std::vector<size_t>)->Range(0, max_iter_range);
BENCHMARK_TEMPLATE(benchmark_construct_iterator_pair, std_containers::vector<size_t>)->Range(0, max_iter_range);
BENCHMARK_TEMPLATE(benchmark_construct_iterator_pair, std::vector<std_containers::vector<char>>)->Range(0, max_iter_range);
BENCHMARK_TEMPLATE(benchmark_construct_iterator_pair, std_containers::vector<std_containers::vector<char>>)->Range(0, max_iter_range);
BENCHMARK_TEMPLATE(benchmark_construct_iterator_pair, std::vector<std::vector<char>>)->Range(0, max_iter_range);
BENCHMARK_TEMPLATE(benchmark_construct_iterator_pair, std_containers::vector<std::vector<char>>)->Range(0, max_iter_range);

template<typename Container>
void benchmark_push_back(benchmark::State & state) {
	for (auto _ : state) {
		auto v = Container();
		v.push_back(42);
		benchmark::DoNotOptimize(v.data());
		benchmark::ClobberMemory();
	}
}

template<typename Container>
void benchmark_resize(benchmark::State & state) {
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

constexpr auto range_max = 1 << 22;
BENCHMARK_TEMPLATE(benchmark_resize, std::vector<int>)->Range(0, range_max);
BENCHMARK_TEMPLATE(benchmark_resize, std_containers::vector<int>)->Range(0, range_max);

} // namespace
