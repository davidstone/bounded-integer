// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <numeric_traits/has_int128.hpp>

export module bounded.arithmetic.multiplies;

import bounded.arithmetic.base;
import bounded.arithmetic.extreme_values;
import bounded.arithmetic.plus;
import bounded.arithmetic.safe_abs;
import bounded.arithmetic.unary_minus;
import bounded.bounded_integer;
import bounded.homogeneous_equals;
import bounded.integer;
import bounded.minmax;
import bounded.safe_extreme;

import numeric_traits;
import std_module;

namespace bounded {

template<auto lhs, auto rhs>
constexpr auto safer_multiply(constant_t<lhs> const &, constant_t<rhs> const &) {
	constexpr auto negative = (lhs < 0) xor (rhs < 0);
	constexpr auto ulhs = static_cast<numeric_traits::max_unsigned_t>(lhs);
	constexpr auto urhs = static_cast<numeric_traits::max_unsigned_t>(rhs);
	constexpr auto modulo_equivalent_result = ulhs * urhs;
	if constexpr (negative) {
		if constexpr (lhs > 0) {
			static_assert(
				-modulo_equivalent_result / ulhs == -urhs,
				"Multiplication requires a larger type than currently supported."
			);
		} else if constexpr (rhs > 0) {
			static_assert(
				-modulo_equivalent_result / urhs == -ulhs,
				"Multiplication requires a larger type than currently supported."
			);
		}
		return static_cast<numeric_traits::max_signed_t>(modulo_equivalent_result);
	} else {
		if constexpr (lhs > 0) {
			static_assert(
				modulo_equivalent_result / ulhs == urhs,
				"Multiplication requires a larger type than currently supported."
			);
		} else if constexpr (lhs < 0) {
			static_assert(
				modulo_equivalent_result / -ulhs == -urhs,
				"Multiplication requires a larger type than currently supported."
			);
		}
		return modulo_equivalent_result;
	}
}

export constexpr auto operator*(bounded_integer auto const lhs_, bounded_integer auto const rhs_) {
	return modulo_equivalent_operator_overload(lhs_, rhs_, std::multiplies(), [](auto const lhs, auto const rhs) {
		constexpr auto p0 = safer_multiply(lhs.min, rhs.min);
		constexpr auto p1 = safer_multiply(lhs.min, rhs.max);
		constexpr auto p2 = safer_multiply(lhs.max, rhs.min);
		constexpr auto p3 = safer_multiply(lhs.max, rhs.max);
		return min_max(
			detail::safe_min(p0, p1, p2, p3),
			detail::safe_max(p0, p1, p2, p3)
		);
	});
}

// Not technically necessary, but improves compile times in some common cases
export constexpr auto operator*(bounded_integer auto const lhs, bounded::constant_t<1>) {
	return lhs;
}
export constexpr auto operator*(bounded::constant_t<1>, bounded_integer auto const rhs) {
	return rhs;
}
export constexpr auto operator*(bounded::constant_t<1>, bounded::constant_t<1>) {
	return bounded::constant<1>;
}

} // namespace bounded

namespace {

static_assert(homogeneous_equals(
	bounded::constant<0> * bounded::constant<0>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<0> * bounded::constant<1000>,
	bounded::constant<0>
));

static_assert(homogeneous_equals(
	signed_max * bounded::constant<1>,
	signed_max
));
static_assert(homogeneous_equals(
	signed_min * bounded::constant<1>,
	signed_min
));
static_assert(homogeneous_equals(
	unsigned_max * bounded::constant<1>,
	unsigned_max
));
static_assert(homogeneous_equals(
	signed_max * bounded::constant<-1>,
	-signed_max
));
static_assert(homogeneous_equals(
	signed_min * bounded::constant<-1>,
	-signed_min
));
static_assert(homogeneous_equals(
	signed_max * bounded::constant<2>,
	signed_max + signed_max
));
static_assert(homogeneous_equals(
	-signed_min * bounded::constant<-1>,
	signed_min
));

#if NUMERIC_TRAITS_HAS_INT128

static_assert(homogeneous_equals(
	bounded::constant<1'000'000'000'000'000'000> * bounded::constant<1'000'000'000'000'000'000>,
	bounded::constant<numeric_traits::uint128_t(1'000'000'000'000'000'000) * numeric_traits::uint128_t(1'000'000'000'000'000'000)>
));

#endif

static_assert(homogeneous_equals(
	bounded::integer<-3, 3>(bounded::constant<1>) * bounded::integer<0, 1>(bounded::constant<1>),
	bounded::integer<-3, 3>(bounded::constant<1>)
));
static_assert(homogeneous_equals(
	bounded::integer<1, 10>(bounded::constant<9>) * bounded::integer<-3, 11>(bounded::constant<4>),
	bounded::integer<-30, 110>(bounded::constant<36>)
));

}	// namespace
