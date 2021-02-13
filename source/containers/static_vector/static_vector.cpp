// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/static_vector/static_vector.hpp>
#include <containers/array/array.hpp>
#include <containers/emplace_back.hpp>
#include <containers/index_type.hpp>

#include "../../test_assert.hpp"
#include "../test_sequence_container.hpp"

namespace {

using namespace bounded::literal;

template<typename T>
using test_static_vector = containers::static_vector<T, 40>;

static_assert(std::is_trivially_copy_constructible_v<test_static_vector<int>>);
static_assert(std::is_trivially_move_constructible_v<test_static_vector<int>>);
static_assert(std::is_trivially_destructible_v<test_static_vector<int>>);
static_assert(std::is_trivially_copy_assignable_v<test_static_vector<int>>);
static_assert(std::is_trivially_move_assignable_v<test_static_vector<int>>);

static_assert(!std::is_trivially_copy_constructible_v<test_static_vector<bounded::test_int>>);
static_assert(!std::is_trivially_move_constructible_v<test_static_vector<bounded::test_int>>);
static_assert(!std::is_trivially_destructible_v<test_static_vector<bounded::test_int>>);
static_assert(!std::is_trivially_copy_assignable_v<test_static_vector<bounded::test_int>>);
static_assert(!std::is_trivially_move_assignable_v<test_static_vector<bounded::test_int>>);

static_assert(containers_test::test_sequence_container<test_static_vector<int>>());

using index_type = containers::index_type<containers::static_vector<int, 10>>;
static_assert(bounded::min_value<index_type> == 0_bi);
static_assert(bounded::max_value<index_type> == 9_bi);
static_assert(!containers::iterator<containers::static_vector<std::string, 6>>);

} // namespace

int main() {
	BOUNDED_TEST(containers_test::test_sequence_container<test_static_vector<bounded::test_int>>());
}
