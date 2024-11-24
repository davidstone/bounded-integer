// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.divides;

import bounded.arithmetic.extreme_values;
import bounded.arithmetic.min_max;
import bounded.arithmetic.safe_abs;
import bounded.arithmetic.unary_minus;
import bounded.bounded_integer;
import bounded.comparison;
import bounded.conditional_function;
import bounded.homogeneous_equals;
import bounded.integer;
import bounded.minmax;
import bounded.normalize;
import bounded.safe_extreme;
import bounded.unchecked;

import numeric_traits;
import std_module;

namespace bounded {

// Ignores divide by 0, caught by constexpr
template<auto lhs, auto rhs>
constexpr auto safer_divide() {
	constexpr auto is_negative = (lhs < 0) xor (rhs < 0);
	constexpr auto positive_result = safe_abs(lhs) / safe_abs(rhs);
	if constexpr (is_negative) {
		return static_cast<numeric_traits::max_signed_t>(-positive_result);
	} else {
		return positive_result;
	}
}

constexpr auto positive_divisor_division(auto const lhs, auto const rhs) {
	constexpr auto minimum_divisor = conditional_function<lhs.min.value() < 0>(rhs.min, rhs.max).value();
	constexpr auto maximum_divisor = conditional_function<lhs.max.value() < 0>(rhs.max, rhs.min).value();
	return min_max(
		safer_divide<lhs.min.value(), minimum_divisor>(),
		safer_divide<lhs.max.value(), maximum_divisor>()
	);
}
constexpr auto negative_divisor_division(auto const lhs, auto const rhs) {
	constexpr auto minimum_divisor = conditional_function<lhs.max.value() >= 0>(rhs.max, rhs.min).value();
	constexpr auto maximum_divisor = conditional_function<lhs.min.value() >= 0>(rhs.min, rhs.max).value();
	return min_max(
		safer_divide<lhs.max.value(), minimum_divisor>(),
		safer_divide<lhs.min.value(), maximum_divisor>()
	);
}

constexpr auto divides_operator_range(auto const lhs, auto const rhs) {
	if constexpr (rhs.min >= constant<1>) {
		return positive_divisor_division(lhs, rhs);
	} else if constexpr (rhs.max <= constant<-1>) {
		return negative_divisor_division(lhs, rhs);
	} else {
		constexpr auto positive = positive_divisor_division(lhs, min_max(constant<1>, rhs.max));
		constexpr auto negative = negative_divisor_division(lhs, min_max(rhs.min, constant<-1>));
		return min_max(
			safe_min(positive.min, negative.min),
			safe_max(positive.max, negative.max)
		);
	}
}

export template<auto lhs_min, auto lhs_max, auto rhs_min, auto rhs_max>
constexpr auto operator/(integer<lhs_min, lhs_max> const lhs, integer<rhs_min, rhs_max> const rhs) {
	static_assert(rhs_min != 0);
	static_assert(rhs_max != 0);
	constexpr auto range = divides_operator_range(
		min_max(constant<lhs_min>, constant<lhs_max>),
		min_max(constant<rhs_min>, constant<rhs_max>)
	);
	using result_t = integer<normalize<range.min>, normalize<range.max>>;
	using common_t = typename std::common_type_t<result_t, integer<lhs_min, lhs_max>, integer<rhs_min, rhs_max>>::underlying_type;
	// It is safe to use the unchecked constructor because we already know that
	// the result will fit in result_t. We have to cast to the intermediate
	// common_t in case result_t is narrower than one of the arguments.
	return result_t(
		static_cast<common_t>(lhs) / static_cast<common_t>(rhs),
		unchecked
	);
}

}	// namespace bounded

static_assert(homogeneous_equals(
	bounded::constant<0> / bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<1> / bounded::constant<1>,
	bounded::constant<1>
));
static_assert(homogeneous_equals(
	bounded::constant<1> / bounded::constant<2>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<0> / bounded::constant<-1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<1> / bounded::constant<-1>,
	bounded::constant<-1>
));
static_assert(homogeneous_equals(
	bounded::constant<-1> / bounded::constant<1>,
	bounded::constant<-1>
));
static_assert(homogeneous_equals(
	bounded::constant<-1> / bounded::constant<-1>,
	bounded::constant<1>
));

static_assert(homogeneous_equals(
	signed_max / bounded::constant<1>,
	signed_max
));
static_assert(homogeneous_equals(
	signed_min / bounded::constant<1>,
	signed_min
));
static_assert(homogeneous_equals(
	unsigned_max / bounded::constant<1>,
	unsigned_max
));
static_assert(homogeneous_equals(
	signed_max / bounded::constant<-1>,
	-signed_max
));
#if 0
static_assert(homogeneous_equals(
	signed_min / bounded::constant<-1>,
	-signed_min
));
#endif
static_assert(homogeneous_equals(
	unsigned_max / bounded::constant<2>,
	signed_max
));
static_assert(homogeneous_equals(
	unsigned_max / signed_max,
	bounded::constant<2>
));

static_assert(homogeneous_equals(
	bounded::integer<7, 100>(bounded::constant<9>) / bounded::integer<3, 5>(bounded::constant<4>),
	bounded::integer<1, 33>(bounded::constant<2>)
));

static_assert(homogeneous_equals(
	bounded::integer<-76, -14>(bounded::constant<-20>) / bounded::integer<4, 20>(bounded::constant<4>),
	bounded::integer<-19, 0>(bounded::constant<-5>)
));

static_assert(homogeneous_equals(
	bounded::integer<7, 100>(bounded::constant<9>) / bounded::integer<-8, -2>(bounded::constant<-4>),
	bounded::integer<-50, 0>(bounded::constant<-2>)
));

static_assert(homogeneous_equals(
	bounded::integer<-76, -14>(bounded::constant<-20>) / bounded::integer<-21, -6>(bounded::constant<-10>),
	bounded::integer<0, 12>(bounded::constant<2>)
));

static_assert(homogeneous_equals(
	bounded::integer<1, 10>(bounded::constant<9>) / bounded::integer<-3, 11>(bounded::constant<4>),
	bounded::integer<-10, 10>(bounded::constant<2>)
));

static_assert(homogeneous_equals(
	bounded::integer<-71, 6>(bounded::constant<-12>) / bounded::integer<5, 12>(bounded::constant<6>),
	bounded::integer<-14, 1>(bounded::constant<-2>)
));

static_assert(homogeneous_equals(
	bounded::integer<-20, 30>(bounded::constant<9>) / bounded::integer<-7, 3>(bounded::constant<2>),
	bounded::integer<-30, 30>(bounded::constant<4>)
));

// constexpr auto fails_to_compile = bounded::constant<1> / bounded::constant<0>;
