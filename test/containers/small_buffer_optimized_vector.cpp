// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <compare>
#include <catch2/catch_test_macros.hpp>

import containers.small_buffer_optimized_vector;
import containers.test_reserve_and_capacity;
import containers.test_sequence_container;
import containers.test_set_size;
import bounded.test_int;

namespace {

TEST_CASE("small_buffer_optimized_vector", "[small_buffer_optimized_vector]") {
	containers_test::test_sequence_container<containers::small_buffer_optimized_vector<int, 3>>();
	containers_test::test_sequence_container<containers::small_buffer_optimized_vector<bounded_test::integer, 3>>();
	containers_test::test_reserve_and_capacity<containers::small_buffer_optimized_vector<int, 3>>();
	containers_test::test_reserve_and_capacity<containers::small_buffer_optimized_vector<bounded_test::integer, 3>>();
	containers_test::test_set_size<containers::small_buffer_optimized_vector<bounded_test::integer, 1>>();
}

} // namespace
