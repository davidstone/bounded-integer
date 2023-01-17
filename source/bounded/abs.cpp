// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.abs;

import bounded.arithmetic.unary_minus;
import bounded.cast;
import bounded.comparison;
import bounded.homogeneous_equals;
import bounded.integer;
import bounded.is_bounded_integer;
import bounded.minmax;

import numeric_traits;

namespace bounded {

export constexpr auto abs(bounded_integer auto const value) {
	using Integer = decltype(value);
	if constexpr (numeric_traits::min_value<Integer> >= constant<0>) {
		return value;
	} else if constexpr (numeric_traits::max_value<Integer> <= constant<0>) {
		return -value;
	} else {
		// The 0 has to be there to restrict the range of possible values. Without
		// it, abs(integer<-7, 3>) would be [-3, 7] instead of [0, 7].
		return increase_min<0>(max(value, -value), unchecked);
	}
}

} // namespace bounded

static_assert(homogeneous_equals(
	bounded::abs(bounded::integer<-10, 4>(bounded::constant<-5>)),
	bounded::integer<0, 10>(bounded::constant<5>)
));
static_assert(homogeneous_equals(
	bounded::abs(bounded::integer<-10, -10>(bounded::constant<-10>)),
	bounded::integer<10, 10>(bounded::constant<10>)
));
static_assert(homogeneous_equals(
	bounded::abs(bounded::integer<0, 0>(bounded::constant<0>)),
	bounded::integer<0, 0>(bounded::constant<0>)
));
static_assert(homogeneous_equals(
	bounded::abs(bounded::integer<-7, 450>(bounded::constant<-1>)),
	bounded::integer<0, 450>(bounded::constant<1>)
));
static_assert(homogeneous_equals(
	bounded::abs(bounded::integer<-7, 450>(bounded::constant<1>)),
	bounded::integer<0, 450>(bounded::constant<1>)
));
