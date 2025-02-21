// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>

import containers.algorithms.compare;
import containers.test.test_reserve_and_capacity;
import containers.test.test_sequence_container;
import containers.test.test_set_size;
import containers.repeat_n;
import containers.small_buffer_optimized_vector;

import bounded;
import bounded.test_int;

namespace {

using namespace bounded::literal;

static_assert(bounded::default_constructible<containers::small_buffer_optimized_vector<int, 40>>);
static_assert(containers_test::test_sequence_container<containers::small_buffer_optimized_vector<int, 40>>());
static_assert(containers_test::test_set_size<containers::small_buffer_optimized_vector<int, 1>>());

template<typename T>
constexpr auto test_sbo_vector_impl(auto const capacity, auto const size) -> void {
	using container = containers::small_buffer_optimized_vector<T, capacity.value()>;
	static_assert(bounded::default_constructible<container>);
	static_assert(containers_test::test_sequence_container<container>());
	static_assert(containers_test::test_reserve_and_capacity<container>());
	constexpr auto generator = containers::repeat_default_n<T>(size);
	static_assert(containers::equal(container(generator), generator));
}

template<typename T>
constexpr auto test_sbo_vector(auto const capacity) -> void {
	test_sbo_vector_impl<T>(capacity, capacity - 1_bi);
	test_sbo_vector_impl<T>(capacity, capacity);
	test_sbo_vector_impl<T>(capacity, capacity + 1_bi);
}

TEST_CASE("small_buffer_optimized_vector", "[small_buffer_optimized_vector]") {
	test_sbo_vector<int>(2_bi);
	test_sbo_vector<int>(3_bi);
	test_sbo_vector<int>(4_bi);
	test_sbo_vector<char>(126_bi);
	test_sbo_vector<char>(127_bi);
	test_sbo_vector<char>(128_bi);
	test_sbo_vector<char>(254_bi);
	test_sbo_vector<char>(255_bi);
	test_sbo_vector<char>(256_bi);
	static_assert(bounded::default_constructible<containers::small_buffer_optimized_vector<bounded_test::integer, 3>>);
	containers_test::test_sequence_container<containers::small_buffer_optimized_vector<bounded_test::integer, 3>>();
	containers_test::test_reserve_and_capacity<containers::small_buffer_optimized_vector<bounded_test::integer, 3>>();
	containers_test::test_set_size<containers::small_buffer_optimized_vector<bounded_test::integer, 1>>();
}

} // namespace
