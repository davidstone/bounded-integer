// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.unary_plus;

import bounded.arithmetic.extreme_values;
import bounded.bounded_integer;
import bounded.homogeneous_equals;
import bounded.integer;

namespace bounded {

export constexpr auto operator+(bounded_integer auto const value) {
	return value;
}

} // namespace bounded

namespace {

static_assert(homogeneous_equals(
	+bounded::constant<0>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	+bounded::constant<-1>,
	bounded::constant<-1>
));
static_assert(homogeneous_equals(
	+signed_max,
	signed_max
));
static_assert(homogeneous_equals(
	+signed_min,
	signed_min
));
static_assert(homogeneous_equals(
	+unsigned_max,
	unsigned_max
));

constexpr auto x = bounded::integer<1, 10>(bounded::constant<9>);
static_assert(homogeneous_equals(
	+x,
	x
));

} // namespace