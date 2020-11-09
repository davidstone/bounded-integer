// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/arithmetic/unary_plus.hpp>
#include <bounded/detail/class.hpp>
#include "extreme_values.hpp"
#include "../../homogeneous_equals.hpp"

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

constexpr auto x = bounded::integer<1, 10>(9, bounded::non_check);
static_assert(homogeneous_equals(
	+x,
	x
));

}	// namespace
