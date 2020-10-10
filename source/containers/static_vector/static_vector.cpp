// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/static_vector/static_vector.hpp>
#include <containers/array/array.hpp>
#include <containers/emplace_back.hpp>
#include <containers/repeat_n.hpp>

#include "../../test_assert.hpp"
#include "../test_sequence_container.hpp"

namespace {

using namespace bounded::literal;

static_assert(std::is_trivially_copy_constructible_v<containers::static_vector<int, 3>>);
static_assert(std::is_trivially_move_constructible_v<containers::static_vector<int, 3>>);
static_assert(std::is_trivially_destructible_v<containers::static_vector<int, 3>>);
static_assert(std::is_trivially_copy_assignable_v<containers::static_vector<int, 3>>);
static_assert(std::is_trivially_move_assignable_v<containers::static_vector<int, 3>>);

template<typename T>
using test_static_vector = containers::static_vector<T, 1000>;

static_assert(containers_test::test_sequence_container_int<test_static_vector>());

using index_type = containers::index_type<containers::static_vector<int, 10>>;
static_assert(bounded::min_value<index_type> == 0_bi);
static_assert(bounded::max_value<index_type> == 9_bi);
static_assert(std::is_same<index_type::overflow_policy, bounded::throw_policy<std::out_of_range>>{});
static_assert(!containers::iterator<containers::static_vector<std::string, 6>>);

static_assert(containers_test::check_non_triviality<test_static_vector>());

} // namespace

int main() {
	BOUNDED_TEST(containers_test::test_sequence_container_recursive<test_static_vector>());
	BOUNDED_TEST(containers_test::test_sequence_container_complex<test_static_vector>());
}
