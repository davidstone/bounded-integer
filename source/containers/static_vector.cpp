// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/static_vector.hpp>

#include <containers/array.hpp>
#include <containers/index_type.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <catch2/catch_test_macros.hpp>

#include "../bounded/homogeneous_equals.hpp"
#include "../test_assert.hpp"
#include "../test_int.hpp"
#include "test_sequence_container.hpp"
#include "test_set_size.hpp"

namespace {

using namespace bounded::literal;

template<typename T>
using test_static_vector = containers::static_vector<T, 40_bi>;

static_assert(test_static_vector<int>::capacity() == 40_bi);

static_assert(std::is_trivially_copy_constructible_v<test_static_vector<int>>);
static_assert(std::is_trivially_move_constructible_v<test_static_vector<int>>);
static_assert(std::is_trivially_destructible_v<test_static_vector<int>>);
static_assert(std::is_trivially_copy_assignable_v<test_static_vector<int>>);
static_assert(std::is_trivially_move_assignable_v<test_static_vector<int>>);

static_assert(!std::is_trivially_copy_constructible_v<test_static_vector<bounded_test::integer>>);
static_assert(!std::is_trivially_move_constructible_v<test_static_vector<bounded_test::integer>>);
static_assert(!std::is_trivially_destructible_v<test_static_vector<bounded_test::integer>>);
static_assert(!std::is_trivially_copy_assignable_v<test_static_vector<bounded_test::integer>>);
static_assert(!std::is_trivially_move_assignable_v<test_static_vector<bounded_test::integer>>);

static_assert(containers_test::test_sequence_container<test_static_vector<int>>());
static_assert(containers_test::test_set_size<test_static_vector<int>>());

using index_type = containers::index_type<containers::static_vector<int, 10_bi>>;
static_assert(numeric_traits::min_value<index_type> == 0_bi);
static_assert(numeric_traits::max_value<index_type> == 9_bi);
static_assert(!containers::iterator<containers::static_vector<std::string, 6_bi>>);

static_assert(homogeneous_equals(
	containers::make_static_vector(containers::array({5, 3})),
	containers::static_vector<int, 2_bi>({5, 3})
));

static_assert(!bounded::constructible_from<containers::static_vector<int, 1_bi>, containers::c_array<int, 2> &&>);

TEST_CASE("static_vector", "[static_vector]") {
	containers_test::test_sequence_container<test_static_vector<bounded_test::integer>>();
	containers_test::test_set_size<test_static_vector<bounded_test::integer>>();
}

} // namespace
