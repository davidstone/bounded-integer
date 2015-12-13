// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "integer_range.hpp"
#include "detail/literal.hpp"

namespace {
using namespace ::bounded::literal;

constexpr auto x = bounded::integer_range(10_bi);
static_assert(
	*x.begin() == 0_bi,
	"Incorrect initial value of range."
);
static_assert(
	x.front() == 0_bi,
	"Incorrect initial value of range."
);
static_assert(
	x.back() == 9_bi,
	"Incorrect initial value of range."
);

static_assert(
	bounded::integer_range(5_bi, 12_bi)[2_bi] == 7_bi,
	"Incorrect indexed value of range that does not start with 0."
);

static_assert(
	bounded::integer_range(0_bi, 0_bi).size() == 0_bi,
	"Incorrect size of empty range."
);
static_assert(
	bounded::integer_range(-5_bi, -5_bi).empty(),
	"Incorrect size of empty range."
);

static_assert(
	bounded::integer_range(
		static_cast<bounded::integer<1, 2>>(1_bi),
		static_cast<bounded::integer<3, 5>>(3_bi)
	).size() == 2_bi,
	"Incorrect size of range with non-overlapping and possibly variable begin and end."
);

}	// namespace
