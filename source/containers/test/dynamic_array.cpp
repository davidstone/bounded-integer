// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.dynamic_array;

import containers.test.test_sequence_container;

import containers.dynamic_array;

import bounded;
import bounded.test_int;
import std_module;

static_assert(containers_test::test_sequence_container<containers::dynamic_array<int>>());
static_assert(containers_test::test_sequence_container<containers::dynamic_array<bounded_test::integer>>());

static_assert(bounded::convertible_to<containers::dynamic_array<bounded_test::integer> const &, std::span<bounded_test::integer const>>);
static_assert(bounded::convertible_to<containers::dynamic_array<bounded_test::integer> &, std::span<bounded_test::integer>>);

namespace {

struct recursive {
	containers::dynamic_array<recursive, bounded::integer<0, 1>> m;
};

} // namespace