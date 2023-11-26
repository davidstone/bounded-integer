// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.plus;

import bounded.arithmetic.base;
import bounded.arithmetic.extreme_values;
import bounded.bounded_integer;
import bounded.comparison;
import bounded.homogeneous_equals;
import bounded.integer;

import numeric_traits;
import std_module;

namespace bounded {

constexpr auto max_signed = numeric_traits::max_value<numeric_traits::max_signed_t>;

template<auto lhs, auto rhs>
constexpr auto safer_add() {
	constexpr auto modulo_equivalent_value = static_cast<numeric_traits::max_unsigned_t>(lhs) + static_cast<numeric_traits::max_unsigned_t>(rhs);
	if constexpr (lhs >= 0 and rhs >= 0) {
		static_assert(
			modulo_equivalent_value >= static_cast<numeric_traits::max_unsigned_t>(lhs) and modulo_equivalent_value >= static_cast<numeric_traits::max_unsigned_t>(rhs),
			"Overflow in calculation of bounds."
		);
		return modulo_equivalent_value;
	} else if constexpr (lhs > max_signed or rhs > max_signed) {
		return modulo_equivalent_value;
	} else {
		static_assert(
			lhs > 0 or rhs > 0 or modulo_equivalent_value > max_signed,
			"Overflow in calculation of bounds."
		);
		return static_cast<numeric_traits::max_signed_t>(modulo_equivalent_value);
	}
}

export constexpr auto operator+(bounded_integer auto const lhs_, bounded_integer auto const rhs_) {
	return detail::modulo_equivalent_operator_overload(lhs_, rhs_, std::plus(), [](auto const lhs, auto const rhs) {
		return detail::min_max{
			::bounded::safer_add<lhs.min.value(), rhs.min.value()>(),
			::bounded::safer_add<lhs.max.value(), rhs.max.value()>()
		};
	});
}

}	// namespace bounded

namespace {

static_assert(homogeneous_equals(
	bounded::constant<0> + bounded::constant<0>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<0> + bounded::constant<1>,
	bounded::constant<1>
));
static_assert(homogeneous_equals(
	bounded::constant<-1> + bounded::constant<1>,
	bounded::constant<0>
));

using numeric_traits::max_unsigned_t;

static_assert(homogeneous_equals(
	signed_max + bounded::constant<1>,
	bounded::constant<static_cast<max_unsigned_t>(signed_max) + 1>
));

static_assert(homogeneous_equals(
	signed_max + signed_max,
	bounded::constant<static_cast<max_unsigned_t>(signed_max) + static_cast<max_unsigned_t>(signed_max)>
));

// Do not use bounded's operator- for this test
constexpr auto negative_signed_min = bounded::constant<-static_cast<max_unsigned_t>(signed_min)>;
static_assert(negative_signed_min == signed_max + bounded::constant<1>);
static_assert(homogeneous_equals(
	signed_min + negative_signed_min,
	bounded::constant<0>
));

static_assert(homogeneous_equals(
	unsigned_max + signed_min,
	bounded::constant<static_cast<max_unsigned_t>(unsigned_max) + static_cast<max_unsigned_t>(signed_min)>
));

static_assert(homogeneous_equals(
	bounded::integer<1, 10>(bounded::constant<9>) + bounded::integer<-3, 11>(bounded::constant<4>),
	bounded::integer<-2, 21>(bounded::constant<13>)
));

}	// namespace
