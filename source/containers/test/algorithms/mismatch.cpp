// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.algorithms.mismatch;

import containers.algorithms.mismatch;
import containers.array;
import containers.begin_end;
import containers.size;

import bounded;
import std_module;

using namespace bounded::literal;

constexpr auto check_mismatch(auto const & r1, auto const & r2, auto const index) -> bool {
	auto check = [=](auto const & lhs, auto const & rhs) {
		return
			containers::mismatch(lhs, rhs) ==
			std::pair(containers::begin(lhs) + index, containers::begin(rhs) + index);
	};
	return check(r1, r2) and check(r2, r1);
}

constexpr auto empty = containers::array<int, 0_bi>();
constexpr auto one = containers::array({1});
constexpr auto two = containers::array({2});
constexpr auto one_two = containers::array({1, 2});
constexpr auto one_two_three_four = containers::array({1, 2, 3, 4});
constexpr auto one_two_four_three = containers::array({1, 2, 4, 3});

static_assert(check_mismatch(empty, empty, 0_bi));

static_assert(check_mismatch(empty, one, 0_bi));
static_assert(check_mismatch(one, one, 1_bi));
static_assert(check_mismatch(one, two, 0_bi));

static_assert(check_mismatch(empty, one_two, 0_bi));
static_assert(check_mismatch(one, one_two, 1_bi));
static_assert(check_mismatch(two, one_two, 0_bi));
static_assert(check_mismatch(one_two, one_two, 2_bi));

static_assert(check_mismatch(one_two, one_two_three_four, 2_bi));
static_assert(check_mismatch(one_two_three_four, one_two_four_three, 2_bi));
