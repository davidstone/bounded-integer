// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.linear_map;

import containers.test.test_associative_container;
import containers.test.test_reserve_and_capacity;

import containers.linear_map;

import bounded.test_int;

using non_copyable_map = containers::linear_map<bounded_test::non_copyable_integer, bounded_test::non_copyable_integer>;

static_assert(containers_test::test_reserve_and_capacity<non_copyable_map>());
static_assert(containers_test::test_associative_container<non_copyable_map>());
