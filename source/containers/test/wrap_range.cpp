// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.wrap_range;

import containers.algorithms.compare;

import containers.array;
import containers.wrap_range;

import bounded;

using namespace bounded::literal;

static_assert(containers::equal(
	containers::wrap_range(containers::array<int, 0_bi>()),
	containers::array<int, 0_bi>()
));
constexpr auto range = containers::array({1, 3, 6, 2});
static_assert(containers::equal(
	containers::wrap_range(range),
	range
));
