// result_type of arithmetic operations
// Copyright (C) 2013 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef RANGED_INTEGER_ARITHMETIC_RESULT_TYPE_HPP_
#define RANGED_INTEGER_ARITHMETIC_RESULT_TYPE_HPP_

#include "common_policy.hpp"
#include "forward_declaration.hpp"
#include "minmax.hpp"
#include "numeric_limits.hpp"

#include <cstdint>
#include <utility>

namespace detail {

// We have to define our own function objects rather than using the classes from
// <functional> because we need constexpr

class plus {
public:
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const noexcept -> decltype(std::forward<LHS>(lhs) + std::forward<RHS>(rhs)) {
		return std::forward<LHS>(lhs) + std::forward<RHS>(rhs);
	}
};
class minus {
public:
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const noexcept -> decltype(std::forward<LHS>(lhs) - std::forward<RHS>(rhs)) {
		return std::forward<LHS>(lhs) - std::forward<RHS>(rhs);
	}
};
class multiplies {
public:
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const noexcept -> decltype(std::forward<LHS>(lhs) * std::forward<RHS>(rhs)) {
		return std::forward<LHS>(lhs) * std::forward<RHS>(rhs);
	}
};
class divides {
public:
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const -> decltype(std::forward<LHS>(lhs) / std::forward<RHS>(rhs)) {
		return std::forward<LHS>(lhs) / std::forward<RHS>(rhs);
	}
};
class modulus {
public:
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const -> decltype(std::forward<LHS>(lhs) % std::forward<RHS>(rhs)) {
		return std::forward<LHS>(lhs) % std::forward<RHS>(rhs);
	}
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max,
	typename Operator
>
class operator_range {
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
class operator_range<lhs_min, lhs_max, rhs_min, rhs_max, plus> {
public:
	static constexpr auto minimum = lhs_min + rhs_min;
	static constexpr auto maximum = lhs_max + rhs_max;
	static_assert(minimum <= maximum, "Range is inverted.");
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
class operator_range<lhs_min, lhs_max, rhs_min, rhs_max, minus> {
public:
	static constexpr auto minimum = lhs_min - rhs_max;
	static constexpr auto maximum = lhs_max - rhs_min;
	static_assert(minimum <= maximum, "Range is inverted.");
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
class operator_range<lhs_min, lhs_max, rhs_min, rhs_max, multiplies> {
private:
	static constexpr auto p0 = lhs_min * rhs_min;
	static constexpr auto p1 = lhs_min * rhs_max;
	static constexpr auto p2 = lhs_max * rhs_min;
	static constexpr auto p3 = lhs_max * rhs_max;
public:
	static constexpr auto minimum = min(p0, p1, p2, p3);
	static constexpr auto maximum = max(p0, p1, p2, p3);
	static_assert(minimum <= maximum, "Range is inverted.");
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
class operator_range<lhs_min, lhs_max, rhs_min, rhs_max, divides> {
private:
	// If 1 falls within the range, that is the least positive divisor. The
	// other options are a range that are entirely positive, in which case I
	// want to return the least value, or the range is entirely negative or
	// zero, in which case I pick the greatest absolute value (which is the
	// minimum) so that the 'positive' divisor is not selected in a later step.
	// We can use similar logic for greatest_negative_divisor.
	static constexpr auto least_positive_divisor = (rhs_min <= 1 and 1 <= rhs_max) ? 1 : rhs_min;
	static constexpr auto greatest_negative_divisor = (rhs_min <= -1 and -1 <= rhs_max) ? -1 : rhs_max;
	static constexpr auto g0 = lhs_min / least_positive_divisor;
	static constexpr auto g1 = lhs_min / greatest_negative_divisor;
	static constexpr auto g2 = lhs_max / least_positive_divisor;
	static constexpr auto g3 = lhs_max / greatest_negative_divisor;

	static constexpr auto l0 = lhs_min / rhs_min;
	static constexpr auto l1 = lhs_min / rhs_max;
	static constexpr auto l2 = lhs_max / rhs_min;
	static constexpr auto l3 = lhs_max / rhs_max;
public:
	static constexpr auto minimum = min(l0, l1, l2, l3, g0, g1, g2, g3);
	static constexpr auto maximum = max(g0, g1, g2, g3);
	static_assert(minimum <= maximum, "Range is inverted.");
};

constexpr std::pair<intmax_t, intmax_t> combine(std::pair<intmax_t, intmax_t> lhs, std::pair<intmax_t, intmax_t> rhs) noexcept {
	return std::make_pair(max(lhs.first, rhs.first), min(lhs.second, rhs.second));
}

constexpr std::pair<intmax_t, intmax_t> modulo_round(intmax_t minimum_dividend, intmax_t maximum_dividend, intmax_t divisor) noexcept {
	// When we divide both ends of our lhs range (the dividend) by a particular
	// value in the rhs range (the divisor) there are two possibilities.
	//
	// If they are not equal, then that means that the periodic modulo function
	// has reset to 0 at some point in that range. The greatest value lies
	// immediately before this resets to 0, but we do not have to calculate
	// where in the range it is. We already know that its magnitude will be one
	// less than the magnitude of the divisor. The least value is obviously 0,
	// as we are ignoring the sign of the dividend for these calculations.
	//
	// If they are equal, then we know that the maximum value is at the top end
	// of the range. The function never reset, and it increases by one for each
	// increase in the dividend until it resets to 0. Since it never reset, the
	// maximum value must be the point at which we have had the most increases.
	// The minimum value is the point at which we have had the fewest increases.
	return (minimum_dividend / divisor == maximum_dividend / divisor) ?
		std::make_pair(minimum_dividend % divisor, maximum_dividend % divisor) :
		std::make_pair(static_cast<intmax_t>(0), divisor + 1);
}

constexpr std::pair<intmax_t, intmax_t> calculate_minmax(intmax_t minimum_dividend, intmax_t maximum_dividend, intmax_t least_divisor, intmax_t divisor, std::pair<intmax_t, intmax_t> value) noexcept {
	return ((value.first == 0 and value.second <= divisor + 1) or divisor > least_divisor) ?
		value :
		calculate_minmax(minimum_dividend, maximum_dividend, least_divisor, divisor + 1, combine(value, modulo_round(minimum_dividend, maximum_dividend, divisor)));
}

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
class operator_range<lhs_min, lhs_max, rhs_min, rhs_max, modulus> {
private:
	// The sign of the result is equal to the sign of the lhs. The sign of the
	// rhs does not matter.
	//
	// We must be careful when negating due to the possibility of overflow.
	//
	// This operates on negatives until the end to avoid overflow when
	// value == std::numeric_limits<intmax_t>::min()
	// Therefore, all temporaries should be considered to have an implicit
	// "magnitude" and "negative" in their name.

	// The divisor range cannot terminate on a 0 since that is an invalid value.
	static constexpr auto greatest_divisor = (rhs_max < 0) ? rhs_min : min(rhs_min, -rhs_max, -1);
	static constexpr auto least_divisor =
		(rhs_min > 0) ? -rhs_min :
		(rhs_max < 0) ? rhs_max :
		min(max(rhs_min, -rhs_max), -1);
	static_assert(greatest_divisor < 0, "Got a positive value where a negative was expected.");
	static_assert(least_divisor < 0, "Got a positive value where a negative was expected.");

	template<intmax_t minimum_dividend, intmax_t maximum_dividend>
	class sign_free_value {
	public:
		static_assert(minimum_dividend <= 0, "Got a positive value where a negative was expected.");
		static_assert(maximum_dividend <= 0, "Got a positive value where a negative was expected.");
		static_assert(minimum_dividend >= maximum_dividend, "Range is inverted.");
		static constexpr auto value = (greatest_divisor == -1) ?
			std::make_pair(static_cast<intmax_t>(0), static_cast<intmax_t>(0)) :
			calculate_minmax(
				minimum_dividend, maximum_dividend,
				least_divisor, greatest_divisor,
				std::make_pair(std::numeric_limits<intmax_t>::min(), std::numeric_limits<intmax_t>::max())
			);
		static_assert(value.first <= 0, "Got a positive value where a negative was expected.");
		static_assert(value.second <= 0, "Got a positive value where a negative was expected.");
		static_assert(value.first >= value.second, "Range is inverted.");
	};

	static constexpr auto maybe_most_negative_dividend = lhs_min;
	static constexpr auto maybe_least_negative_dividend = (lhs_max < 0) ? lhs_max : max(lhs_min, 0);
	static constexpr auto maybe_most_positive_dividend = lhs_max;
	static constexpr auto maybe_least_positive_dividend = (lhs_min > 0) ? lhs_min : min(lhs_max, 0);

	static constexpr bool has_positive_values = maybe_most_positive_dividend > 0;
	static constexpr bool has_negative_values = maybe_most_negative_dividend <= 0;

	static_assert(has_positive_values or has_negative_values, "The range must contain at least positive or negative / 0 values");

	// Avoid instantiating a template with unexpected values
	static constexpr auto most_negative_dividend = maybe_most_negative_dividend * (has_negative_values ? 1 : 0);
	static constexpr auto least_negative_dividend = maybe_least_negative_dividend * (has_negative_values ? 1 : 0);
	static constexpr auto most_positive_dividend = maybe_most_positive_dividend * (has_positive_values ? 1 : 0);
	static constexpr auto least_positive_dividend = maybe_least_positive_dividend * (has_positive_values ? 1 : 0);
	
	static constexpr auto negative = sign_free_value<least_negative_dividend, most_negative_dividend>::value;
	static constexpr auto positive = sign_free_value<-least_positive_dividend, -most_positive_dividend>::value;
public:
	static_assert(positive.first >= -std::numeric_limits<intmax_t>::max(), "Positive values out of range.");
	static_assert(positive.second >= -std::numeric_limits<intmax_t>::max(), "Positive values out of range.");
	static constexpr auto minimum =
		!has_positive_values ? negative.second :
		!has_negative_values ? -positive.first :
		min(negative.second, -positive.first);
	static constexpr auto maximum =
		!has_positive_values ? negative.first :
		!has_negative_values ? -positive.second :
		max(negative.first, -positive.second);
	static_assert(minimum <= maximum, "Range is inverted.");
};

}	// namespace detail



template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow,
	typename Operator
>
using operator_result = typename common_policy<lhs_overflow, rhs_overflow>::template type<
	detail::operator_range<lhs_min, lhs_max, rhs_min, rhs_max, Operator>::minimum,
	detail::operator_range<lhs_min, lhs_max, rhs_min, rhs_max, Operator>::maximum
>;

template<template<intmax_t, intmax_t> class result_overflow_policy, typename integer>
using unary_minus_result = ranged_integer<-static_cast<intmax_t>(std::numeric_limits<integer>::max()), -static_cast<intmax_t>(std::numeric_limits<integer>::min()), result_overflow_policy>;

#endif	// RANGED_INTEGER_ARITHMETIC_RESULT_TYPE_HPP_
