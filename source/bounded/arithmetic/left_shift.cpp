// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.left_shift;

import bounded.arithmetic.base;
import bounded.bounded_integer;
import bounded.homogeneous_equals;
import bounded.integer;
import bounded.normalize;
import numeric_traits;

namespace bounded {

// Surprised this isn't standardized...
struct left_shift {
	static constexpr auto operator()(auto const lhs, auto const rhs) {
		return lhs << rhs;
	}
};

export constexpr auto operator<<(bounded_integer auto const lhs_, bounded_integer auto const rhs_) {
	return operator_overload(lhs_, rhs_, left_shift(), [](auto const lhs, auto const rhs) {
		// TODO: Broaden range
		return min_max(
			static_cast<numeric_traits::max_signed_t>(lhs.min) << rhs.min.value(),
			static_cast<numeric_traits::max_signed_t>(lhs.max) << rhs.max.value()
		);
	});
}

} // namespace bounded

static_assert(homogeneous_equals(
	bounded::integer<0, 2>(bounded::constant<1>) << bounded::integer<0, 60>(bounded::constant<3>),
	bounded::integer<0, bounded::normalize<(2LL << 60LL)>>(bounded::constant<(1 << 3)>)
));
