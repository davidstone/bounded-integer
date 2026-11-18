// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.right_shift;

import bounded.bounded_integer;
import bounded.homogeneous_equals;
import bounded.integer;
import bounded.normalize;
import bounded.unchecked;

import numeric_traits;
import std_module;

namespace bounded {

export template<auto lhs_min, auto lhs_max, auto rhs_min, auto rhs_max>
constexpr auto operator>>(integer<lhs_min, lhs_max> const lhs, integer<rhs_min, rhs_max> const rhs) {
	// TODO: Broaden range
	using result_t = integer<
		normalize<(static_cast<numeric_traits::max_signed_t>(lhs_min) >> rhs_max)>,
		normalize<(static_cast<numeric_traits::max_signed_t>(lhs_max) >> rhs_min)>
	>;
	using common_t = typename std::common_type_t<result_t, integer<lhs_min, lhs_max>, integer<rhs_min, rhs_max>>::underlying_type;
	// It is safe to use the unchecked constructor because we already know that
	// the result will fit in result_t. We have to cast to the intermediate
	// common_t in case result_t is narrower than one of the arguments.
	return result_t(
		static_cast<common_t>(lhs) >> static_cast<common_t>(rhs),
		unchecked
	);
}

} // namespace bounded

static_assert(homogeneous_equals(
	bounded::constant<100> >> bounded::integer<0, 50>(bounded::constant<1>),
	bounded::integer<0, 100>(bounded::constant<(100 >> 1)>)
));
