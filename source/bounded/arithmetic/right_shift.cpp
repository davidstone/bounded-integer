// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.right_shift;

import bounded.arithmetic.base;
import bounded.bounded_integer;
import bounded.homogeneous_equals;
import bounded.integer;
import numeric_traits;

namespace bounded {

// Surprised this isn't standardized...
struct right_shift {
	static constexpr auto operator()(auto const lhs, auto const rhs) {
		return lhs >> rhs;
	}
};

export constexpr auto operator>>(bounded_integer auto const lhs_, bounded_integer auto const rhs_) {
	return operator_overload(lhs_, rhs_, right_shift(), [](auto const lhs, auto const rhs) {
		// TODO: Broaden range
		return min_max(
			static_cast<numeric_traits::max_signed_t>(lhs.min) >> rhs.max.value(),
			static_cast<numeric_traits::max_signed_t>(lhs.max) >> rhs.min.value()
		);
	});
}

} // namespace bounded

static_assert(homogeneous_equals(
	bounded::constant<100> >> bounded::integer<0, 50>(bounded::constant<1>),
	bounded::integer<0, 100>(bounded::constant<(100 >> 1)>)
));
