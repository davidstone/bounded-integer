// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.algorithms.chunk_by;

import containers.algorithms.all_any_none;
import containers.algorithms.chunk_by;
import containers.algorithms.compare;
import containers.algorithms.zip;

import containers.array;
import containers.vector;

import bounded;
import std_module;

import containers.linear_size;

using namespace bounded::literal;

constexpr auto inner_ranges_equal(auto const & lhs, auto const & rhs) -> bool {
	return containers::all(
		containers::zip(lhs, rhs),
		[](auto const & ranges) { return containers::equal(ranges[0_bi], ranges[1_bi]); }
	);
}

static_assert(inner_ranges_equal(
	containers::chunk_by(containers::array<int, 0_bi>(), std::less()),
	containers::array<containers::array<int, 0_bi>, 0_bi>()
));
static_assert(inner_ranges_equal(
	containers::chunk_by(containers::array({1}), std::less()),
	containers::array({containers::array({1})})
));
static_assert(inner_ranges_equal(
	containers::chunk_by(containers::array({1, 2}), std::less()),
	containers::array({containers::array({1, 2})})
));
static_assert(inner_ranges_equal(
	containers::chunk_by(containers::array({1, 0}), std::less()),
	containers::array({containers::array({1}), containers::array({0})})
));
static_assert(inner_ranges_equal(
	containers::chunk_by(containers::array({1, 0, 1}), std::less()),
	containers::array({containers::vector({1}), containers::vector({0, 1})})
));
