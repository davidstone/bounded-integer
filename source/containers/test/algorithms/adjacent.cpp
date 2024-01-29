// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.algorithms.adjacent;

import containers.algorithms.adjacent;
import containers.algorithms.compare;

import containers.array;

import bounded;
import tv;

using namespace bounded::literal;

static_assert(containers::equal(
	containers::adjacent(containers::array<int, 0_bi>(), 1_bi),
	containers::array<tv::tuple<int>, 0_bi>()
));
static_assert(containers::equal(
	containers::adjacent(containers::array({1}), 1_bi),
	containers::array({tv::tuple(1)})
));
static_assert(containers::equal(
	containers::adjacent(containers::array({1}), 2_bi),
	containers::array<tv::tuple<int, int>, 0_bi>()
));
static_assert(containers::equal(
	containers::adjacent(containers::array({1, 2}), 1_bi),
	containers::array({tv::tuple(1), tv::tuple(2)})
));
static_assert(containers::equal(
	containers::adjacent(containers::array({1, 2}), 2_bi),
	containers::array({tv::tuple(1, 2)})
));
static_assert(containers::equal(
	containers::adjacent(containers::array({1, 2}), 3_bi),
	containers::array<tv::tuple<int, int, int>, 0_bi>()
));
