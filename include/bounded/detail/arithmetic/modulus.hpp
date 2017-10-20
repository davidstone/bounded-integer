// Copyright David Stone 2015.
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
constexpr auto overlap(LHS const & lhs, RHS const & rhs) noexcept {
	return min_max(max(lhs.min, rhs.min), min(lhs.max, rhs.max));
}

template<typename Dividend, typename Divisor>
constexpr auto modulo_round(Dividend const & dividend, Divisor const & divisor) noexcept {
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
		min_max(static_cast<Divisor>(0), divisor + 1);
}

template<typename Dividend, typename Divisor>
constexpr auto sign_free_value(Dividend const & dividend, Divisor divisor) noexcept {
	// I have the special case for -1 to avoid any possibility of
	// integer overflow from std::numeric_limits<intmax_t>::min() / -1
	if (divisor.max == -1) {
		return min_max(static_cast<intmax_t>(0), static_cast<intmax_t>(0));
	}

	auto current = min_max(std::numeric_limits<intmax_t>::min(), std::numeric_limits<intmax_t>::max());
	while (!(current.min == 0 and current.max <= divisor.max + 1) and divisor.max <= divisor.min) {
		current = overlap(current, modulo_round(dividend, divisor.max));
		++divisor.max;
	}
	return current;
}

template<typename LHS, typename RHS>
constexpr auto modulus_operator_range(LHS const & lhs, RHS const & rhs) noexcept {
	// The sign of the result is equal to the sign of the lhs. The sign of the
	// rhs does not matter.
	//
	// We must be careful when negating due to the possibility of overflow.
	//
	// This operates on negatives until the end to avoid overflow when
	// value == std::numeric_limits<intmax_t>::min()
	// Therefore, all temporaries should be considered to have an implicit
	// "magnitude" and "negative" in their name.
	//
	// The divisor range cannot terminate on a 0 since that is an invalid value.
	auto const divisor = min_max(
		(rhs.min > 0) ? -rhs.min : (rhs.max < 0) ? rhs.max : -1,
		(rhs.max < 0) ? rhs.min : min(rhs.min, -rhs.max)
	);

	// Avoid instantiating a template with unexpected values
	auto const negative_dividend = min_max(bounded::min(lhs.max, 0), bounded::min(lhs.min, 0));
	auto const positive_dividend = min_max(-bounded::max(lhs.min, 0), -bounded::max(lhs.max, 0));
	
	auto const negative = sign_free_value(negative_dividend, divisor);
	auto const positive = sign_free_value(positive_dividend, divisor);

	auto const has_negative_values = lhs.min <= 0;
	auto const has_positive_values = lhs.max > 0;

	return min_max(
		has_negative_values ? negative.max : -positive.min,
		has_positive_values ? -positive.max : negative.min
	);
}

}	// namespace detail

BOUNDED_INTEGER_OPERATOR_OVERLOADS(%, detail::modulus_operator_range)

}	// namespace bounded

