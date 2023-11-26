// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.modulus;

import bounded.arithmetic.base;
import bounded.arithmetic.extreme_values;
import bounded.arithmetic.safe_abs;
import bounded.bounded_integer;
import bounded.comparison;
import bounded.minmax;
import bounded.homogeneous_equals;
import bounded.integer;

import numeric_traits;
import std_module;

namespace bounded {

constexpr auto update_modulo_range = [](auto const lhs, auto const rhs) {
	return detail::min_max{min(lhs.min, rhs.min), max(lhs.max, rhs.max)};
};

constexpr auto modulo_round = [](auto const dividend, numeric_traits::max_unsigned_t const divisor) {
	// When we divide both ends of the dividend by a particular value in
	// the range of the divisor there are two possibilities:
	return (dividend.min / divisor == dividend.max / divisor) ?
		// If they are equal, the maximum value is at the top end of the range.
		// The function never reset, and it increases by one for each increase
		// in the dividend until it resets. The maximum value is the point at
		// which there were the most increases. The minimum value is the point
		// at which there were the fewest increases.
		detail::min_max{dividend.min % divisor, dividend.max % divisor} :
		// If the quotients are not equal, that means that the periodic modulo
		// function reset to 0 somewhere in that range. The greatest value is
		// immediately before this reset, but we do not have to find where in
		// the range it is. The magnitude will be one less than the magnitude
		// of the divisor. The least value is obviously 0, as we are ignoring
		// the sign of the dividend for these calculations.
		detail::min_max{static_cast<numeric_traits::max_unsigned_t>(0), divisor - 1};
};

constexpr auto sign_free_value = [](auto const dividend, auto divisor) {
	if (divisor.min > dividend.max) {
		return dividend;
	}
	// Intentionally make the min the highest value and the max the smallest
	// value. This allows us to use them as the initial value in an
	// accumulation, as they will "lose" to any value.
	auto current = detail::min_max{
		numeric_traits::max_value<numeric_traits::max_unsigned_t>,
		numeric_traits::min_value<numeric_traits::max_unsigned_t>
	};
	while (current.min != 0 or (current.max != divisor.max - 1 and current.max != dividend.max)) {
		current = update_modulo_range(current, modulo_round(dividend, divisor.min));
		if (divisor.min == divisor.max) {
			break;
		}
		++divisor.min;
	}
	return current;
};

// TODO: make this a lambda after resolution of
// https://github.com/llvm/llvm-project/issues/59513
struct modulus_operator_range_t {
	static constexpr auto operator()(auto const lhs_, auto const rhs_) {
		constexpr auto lhs = detail::min_max{decltype(lhs_.min){}, decltype(lhs_.max){}};
		constexpr auto rhs = detail::min_max{decltype(rhs_.min){}, decltype(rhs_.max){}};
		// The sign of the result is equal to the sign of the lhs. The sign of the
		// rhs does not matter. Therefore, we use the absolute value of the rhs; we
		// must be careful when negating due to the possibility of overflow.
		//
		// The divisor range cannot terminate on a 0 since that is an invalid value.
		constexpr auto divisor = detail::min_max{
			(rhs.min > constant<0>) ? static_cast<numeric_traits::max_unsigned_t>(rhs.min) : (rhs.max < constant<0>) ? -static_cast<numeric_traits::max_unsigned_t>(rhs.max) : 1,
			max(detail::safe_abs(rhs.min.value()), detail::safe_abs(rhs.max.value()))
		};

		constexpr auto negative_dividend = detail::min_max{
			lhs.max < constant<0> ? detail::safe_abs(lhs.max.value()) : 0,
			lhs.min < constant<0> ? detail::safe_abs(lhs.min.value()) : 0
		};
		constexpr auto positive_dividend = detail::min_max{
			static_cast<numeric_traits::max_unsigned_t>(max(lhs.min, constant<0>)),
			static_cast<numeric_traits::max_unsigned_t>(max(lhs.max, constant<0>))
		};
		
		constexpr auto negative = sign_free_value(negative_dividend, divisor);
		constexpr auto positive = sign_free_value(positive_dividend, divisor);

		constexpr auto has_negative_values = lhs.min < constant<0>;
		constexpr auto has_positive_values = lhs.max > constant<0>;
		
		static_assert(not has_negative_values or negative.max <= -static_cast<numeric_traits::max_unsigned_t>(numeric_traits::min_value<numeric_traits::max_signed_t>));

		using min_type = std::conditional_t<has_negative_values or positive.min <= numeric_traits::max_value<numeric_traits::max_signed_t>, numeric_traits::max_signed_t, numeric_traits::max_unsigned_t>;
		using max_type = std::conditional_t<not has_positive_values or positive.max <= numeric_traits::max_value<numeric_traits::max_signed_t>, numeric_traits::max_signed_t, numeric_traits::max_unsigned_t>;
		return detail::min_max{
			has_negative_values ? static_cast<min_type>(-static_cast<numeric_traits::max_signed_t>(negative.max)) : static_cast<min_type>(positive.min),
			has_positive_values ? static_cast<max_type>(positive.max) : static_cast<max_type>(-static_cast<numeric_traits::max_signed_t>(negative.min))
		};
	}
};
constexpr auto modulus_operator_range = modulus_operator_range_t();

export constexpr auto operator%(bounded_integer auto const lhs, bounded_integer auto const rhs) {
	return detail::operator_overload(lhs, rhs, std::modulus(), modulus_operator_range);
}

} // namespace bounded

static_assert(homogeneous_equals(
	bounded::constant<0> % bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<10> % bounded::constant<11>,
	bounded::constant<10>
));
static_assert(homogeneous_equals(
	bounded::constant<10> % bounded::constant<9>,
	bounded::constant<1>
));
static_assert(homogeneous_equals(
	bounded::constant<9> % bounded::constant<11>,
	bounded::constant<9>
));
static_assert(homogeneous_equals(
	bounded::constant<11> % bounded::constant<9>,
	bounded::constant<2>
));
static_assert(homogeneous_equals(
	bounded::constant<13> % bounded::constant<6>,
	bounded::constant<1>
));

static_assert(homogeneous_equals(
	signed_min % bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	signed_max % bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	unsigned_max % bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::integer<17, 23>(bounded::constant<20>) % bounded::integer<-54, -6>(bounded::constant<-33>),
	bounded::integer<0, 23>(bounded::constant<20 % -33>)
));
static_assert(homogeneous_equals(
	bounded::integer<-54, -6>(bounded::constant<-33>) % bounded::integer<17, 23>(bounded::constant<20>),
	bounded::integer<-22, 0>(bounded::constant<-33 % 20>)
));
static_assert(homogeneous_equals(
	bounded::integer<-22, 0>(bounded::constant<-33 % 20>) % bounded::integer<0, 23>(bounded::constant<20 % -33>),
	bounded::integer<-22, 0>(bounded::constant<-13>)
));
static_assert(homogeneous_equals(
	bounded::integer<0, 10>(bounded::constant<10>) % bounded::constant<6>,
	bounded::integer<0, 5>(bounded::constant<4>)
));
static_assert(homogeneous_equals(
	bounded::constant<0> % bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::integer<73, 76>(bounded::constant<73>) % bounded::integer<7, 8>(bounded::constant<7>),
	bounded::integer<1, 6>(bounded::constant<73 % 7>)
));
static_assert(homogeneous_equals(
	bounded::integer<74, 75>(bounded::constant<74>) % bounded::integer<7, 8>(bounded::constant<7>),
	bounded::integer<2, 5>(bounded::constant<74 % 7>)
));

// auto undefined = 1 % zero;

#if 0	
// The algorithm to compute the bounds currently runs in O(n) compile time,
// and thus this test exceed's the constexpr evaluation limits.
constexpr auto bounded_max_range = bounded::integer(numeric_traits::min_value<bounded::detail::signed_max_t>);
static_assert(homogeneous_equals(
	bounded_max_range % bounded_max_range,
	bounded::integer<numeric_traits::min_value<signed_max_t> + 1, -(numeric_traits::min_value<signed_max_t> + 1)>(0)
));
#endif