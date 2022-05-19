// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/small_buffer_optimized_vector.hpp>

#include <catch2/catch_test_macros.hpp>

#include "../test_int.hpp"

#include "test_reserve_and_capacity.hpp"
#include "test_sequence_container.hpp"
#include "test_set_size.hpp"

namespace {

static_assert(containers_test::test_sequence_container<containers::small_buffer_optimized_vector<int, 40>>());
static_assert(containers_test::test_set_size<containers::small_buffer_optimized_vector<int, 1>>());

TEST_CASE("small_buffer_optimized_vector", "[small_buffer_optimized_vector]") {
	containers_test::test_sequence_container<containers::small_buffer_optimized_vector<int, 3>>();
	containers_test::test_sequence_container<containers::small_buffer_optimized_vector<bounded_test::integer, 3>>();
	containers_test::test_reserve_and_capacity<containers::small_buffer_optimized_vector<int, 3>>();
	containers_test::test_reserve_and_capacity<containers::small_buffer_optimized_vector<bounded_test::integer, 3>>();
	containers_test::test_set_size<containers::small_buffer_optimized_vector<bounded_test::integer, 1>>();
}

} // namespace
