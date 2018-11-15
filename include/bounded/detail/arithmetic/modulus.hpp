// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>
#include <bounded/detail/arithmetic/safe_abs.hpp>
#include <bounded/detail/basic_numeric_limits.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/max_builtin.hpp>
#include <bounded/detail/minmax.hpp>

namespace bounded {
namespace detail {

constexpr auto update_modulo_range = [](auto const lhs, auto const rhs) noexcept {
	return min_max{min(lhs.min, rhs.min), max(lhs.max, rhs.max)};
};

constexpr auto modulo_round = [](auto const dividend, max_unsigned_t const divisor) noexcept {
	// When we divide both ends of the dividend by a particular value in
	// the range of the divisor there are two possibilities:
	return (dividend.min / divisor == dividend.max / divisor) ?
		// If they are equal, the maximum value is at the top end of the range.
		// The function never reset, and it increases by one for each increase
		// in the dividend until it resets. The maximum value is the point at
		// which there were the most increases. The minimum value is the point
		// at which there were the fewest increases.
		min_max{dividend.min % divisor, dividend.max % divisor} :
		// If the quotients are not equal, that means that the periodic modulo
		// function reset to 0 somewhere in that range. The greatest value is
		// immediately before this reset, but we do not have to find where in
		// the range it is. The magnitude will be one less than the magnitude
		// of the divisor. The least value is obviously 0, as we are ignoring
		// the sign of the dividend for these calculations.
		min_max{static_cast<max_unsigned_t>(0), divisor - 1};
};

constexpr auto sign_free_value = [](auto const dividend, auto divisor) noexcept {
	if (divisor.min > dividend.max) {
		return dividend;
	}
	// Intentionally make the min the highest value and the max the smallest
	// value. This allows us to use them as the initial value in an
	// accumulation, as they will "lose" to any value.
	auto current = min_max{
		basic_numeric_limits<max_unsigned_t>::max(),
		static_cast<max_unsigned_t>(basic_numeric_limits<max_unsigned_t>::min())
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

constexpr auto modulus_operator_range = [](auto const lhs_, auto const rhs_) noexcept {
	constexpr auto lhs = min_max{decltype(lhs_.min){}, decltype(lhs_.max){}};
	constexpr auto rhs = min_max{decltype(rhs_.min){}, decltype(rhs_.max){}};
	// The sign of the result is equal to the sign of the lhs. The sign of the
	// rhs does not matter. Therefore, we use the absolute value of the rhs; we
	// must be careful when negating due to the possibility of overflow.
	//
	// The divisor range cannot terminate on a 0 since that is an invalid value.
	constexpr auto divisor = min_max{
		(rhs.min > constant<0>) ? static_cast<max_unsigned_t>(rhs.min) : (rhs.max < constant<0>) ? -static_cast<max_unsigned_t>(rhs.max) : 1,
		max(safe_abs(rhs.min.value()), safe_abs(rhs.max.value()))
	};

	constexpr auto negative_dividend = min_max{
		lhs.max < constant<0> ? safe_abs(lhs.max.value()) : 0,
		lhs.min < constant<0> ? safe_abs(lhs.min.value()) : 0
	};
	constexpr auto positive_dividend = min_max{
		static_cast<max_unsigned_t>(max(lhs.min, constant<0>)),
		static_cast<max_unsigned_t>(max(lhs.max, constant<0>))
	};
	
	constexpr auto negative = sign_free_value(negative_dividend, divisor);
	constexpr auto positive = sign_free_value(positive_dividend, divisor);

	constexpr auto has_negative_values = lhs.min < constant<0>;
	constexpr auto has_positive_values = lhs.max > constant<0>;
	
	using signed_limits = basic_numeric_limits<max_signed_t>;
	static_assert(not has_negative_values or negative.max <= -static_cast<max_unsigned_t>(signed_limits::min()));

	using min_type = std::conditional_t<has_negative_values or positive.min <= signed_limits::max(), max_signed_t, max_unsigned_t>;
	using max_type = std::conditional_t<not has_positive_values or positive.max <= signed_limits::max(), max_signed_t, max_unsigned_t>;
	return min_max{
		has_negative_values ? static_cast<min_type>(-static_cast<max_signed_t>(negative.max)) : static_cast<min_type>(positive.min),
		has_positive_values ? static_cast<max_type>(positive.max) : static_cast<max_type>(-static_cast<max_signed_t>(negative.min))
	};
};

}	// namespace detail

BOUNDED_INTEGER_OPERATOR_OVERLOADS(%, detail::modulus_operator_range)

}	// namespace bounded
