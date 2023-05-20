// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.vector;

import containers.test.test_reserve_and_capacity;
import containers.test.test_sequence_container;
import containers.test.test_set_size;

import containers.vector;

import bounded;
import bounded.test_int;
import std_module;

static_assert(containers_test::test_sequence_container<containers::vector<int>>());
static_assert(containers_test::test_sequence_container<containers::vector<bounded_test::integer>>());

static_assert(containers_test::test_reserve_and_capacity<containers::vector<int>>());
static_assert(containers_test::test_reserve_and_capacity<containers::vector<bounded_test::integer>>());

static_assert(containers_test::test_set_size<containers::vector<int>>());
static_assert(containers_test::test_set_size<containers::vector<bounded_test::integer>>());

static_assert(bounded::convertible_to<containers::vector<bounded_test::integer> const &, std::span<bounded_test::integer const>>);
static_assert(bounded::convertible_to<containers::vector<bounded_test::integer> &, std::span<bounded_test::integer>>);

struct recursive {
	containers::vector<recursive, 1> m;
};
