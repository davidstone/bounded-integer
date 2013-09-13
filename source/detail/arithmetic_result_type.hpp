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
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
class operator_range<lhs_min, lhs_max, rhs_min, rhs_max, minus> {
public:
	static constexpr auto minimum = lhs_min - rhs_max;
	static constexpr auto maximum = lhs_max - rhs_min;
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
