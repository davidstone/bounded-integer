// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.array_test;

import containers.array;
import containers.test_sequence_container;

import bounded;

using namespace bounded::literal;

static_assert(containers_test::test_sequence_container_default_constructed_empty<containers::array<int, 0_bi>>());
static_assert(containers_test::test_sequence_container_implicit_from_two_empty_braces<containers::array<int, 0_bi>>());
static_assert(containers_test::test_special_members<containers::array<int, 0_bi>>([] { return containers::array<int, 0_bi>({}); }));
static_assert(containers_test::test_special_members<containers::array<int, 5_bi>>([] { return containers::array({0, 1, 2, 3, 4}); }));
