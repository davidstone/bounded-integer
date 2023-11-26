// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.divides;

import bounded.arithmetic.base;
import bounded.arithmetic.extreme_values;
import bounded.arithmetic.safe_abs;
import bounded.arithmetic.unary_minus;
import bounded.bounded_integer;
import bounded.comparison;
import bounded.homogeneous_equals;
import bounded.integer;
import bounded.minmax;
import bounded.safe_extreme;

import numeric_traits;
import std_module;

namespace bounded {

template<bool condition>
constexpr auto conditional_function(auto if_true, auto if_false) {
	if constexpr (condition) {
		return if_true;
	} else {
		return if_false;
	}
};

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

// TODO: make this a lambda after resolution of
// https://github.com/llvm/llvm-project/issues/59513
struct divides_operator_range_t {
	static constexpr auto operator()(auto const lhs, auto const rhs) {
		static_assert(rhs.min != constant<0>);
		static_assert(rhs.max != constant<0>);
		if constexpr (rhs.min >= constant<1>) {
			return positive_divisor_division(lhs, rhs);
		} else if constexpr (rhs.max <= constant<-1>) {
			return negative_divisor_division(lhs, rhs);
		} else {
			constexpr auto positive = positive_divisor_division(lhs, min_max(constant<1>, rhs.max));
			constexpr auto negative = negative_divisor_division(lhs, min_max(rhs.min, constant<-1>));
			return min_max(
				detail::safe_min(positive.min, negative.min),
				detail::safe_max(positive.max, negative.max)
			);
		}
	}
};
constexpr auto divides_operator_range = divides_operator_range_t();

export constexpr auto operator/(bounded_integer auto const lhs, bounded_integer auto const rhs) {
	return operator_overload(lhs, rhs, std::divides(), divides_operator_range);
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
