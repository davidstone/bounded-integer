// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>

#include <bounded/detail/minmax.hpp>

#include <limits>

namespace bounded {
namespace detail {

template<typename LHS, typename RHS>
constexpr auto update_modulo_range(LHS const lhs, RHS const rhs) noexcept {
	return min_max(min(lhs.min, rhs.min), max(lhs.max, rhs.max));
}

template<typename Dividend>
constexpr auto modulo_round(Dividend const dividend, std::uintmax_t const divisor) noexcept {
	// When we divide both ends of the dividend by a particular value in
	// the range of the divisor there are two possibilities:
	return (dividend.min / divisor == dividend.max / divisor) ?
		// If they are equal, the maximum value is at the top end of the range.
		// The function never reset, and it increases by one for each increase
		// in the dividend until it resets. The maximum value is the point at
		// which there were the most increases. The minimum value is the point
		// at which there were the fewest increases.
		min_max(dividend.min % divisor, dividend.max % divisor) :
		// If the quotients are not equal, that means that the periodic modulo
		// function reset to 0 somewhere in that range. The greatest value is
		// immediately before this reset, but we do not have to find where in
		// the range it is. The magnitude will be one less than the magnitude
		// of the divisor. The least value is obviously 0, as we are ignoring
		// the sign of the dividend for these calculations.
		min_max(static_cast<std::uintmax_t>(0), divisor - 1);
}

template<typename Dividend, typename Divisor>
constexpr auto sign_free_value(Dividend const dividend, Divisor divisor) noexcept {
	if (divisor.min > dividend.max) {
		return dividend;
	}
	// Intentionally make the min the highest value and the max the smallest
	// value. This allows us to use them as the initial value in an
	// accumulation, as they will "lose" to any value.
	auto current = min_max(std::numeric_limits<std::uintmax_t>::max(), std::numeric_limits<std::uintmax_t>::min());
	while (current.min != 0 or (current.max != divisor.max - 1 and current.max != dividend.max)) {
		current = update_modulo_range(current, modulo_round(dividend, divisor.min));
		if (divisor.min == divisor.max) {
			break;
		}
		++divisor.min;
	}
	return current;
}

template<typename Integer>
constexpr auto safe_abs(Integer const value) noexcept {
	return (value < 0) ? -static_cast<std::uintmax_t>(value) : static_cast<std::uintmax_t>(value);
}

template<typename LHS, typename RHS>
constexpr auto modulus_operator_range(LHS const & lhs, RHS const & rhs) noexcept {
	// The sign of the result is equal to the sign of the lhs. The sign of the
	// rhs does not matter. Therefore, we use the absolute value of the rhs; we
	// must be careful when negating due to the possibility of overflow.
	//
	// The divisor range cannot terminate on a 0 since that is an invalid value.
	auto const divisor = min_max(
		(rhs.min > 0) ? static_cast<std::uintmax_t>(rhs.min) : (rhs.max < 0) ? -static_cast<std::uintmax_t>(rhs.max) : 1,
		max(safe_abs(rhs.min), safe_abs(rhs.max))
	);

	auto const negative_dividend = min_max(
		lhs.max < 0 ? safe_abs(lhs.max) : 0,
		lhs.min < 0 ? safe_abs(lhs.min) : 0
	);
	auto const positive_dividend = min_max(
		static_cast<std::uintmax_t>(max(lhs.min, 0)),
		static_cast<std::uintmax_t>(max(lhs.max, 0))
	);
	
	auto const negative = sign_free_value(negative_dividend, divisor);
	auto const positive = sign_free_value(positive_dividend, divisor);

	auto const has_negative_values = lhs.min <= 0;
	auto const has_positive_values = lhs.max > 0;

	return min_max(
		has_negative_values ? -static_cast<std::intmax_t>(negative.max) : static_cast<std::intmax_t>(positive.min),
		has_positive_values ? static_cast<std::intmax_t>(positive.max) : -static_cast<std::intmax_t>(negative.min)
	);
}

}	// namespace detail

BOUNDED_INTEGER_OPERATOR_OVERLOADS(%, detail::modulus_operator_range)

}	// namespace bounded