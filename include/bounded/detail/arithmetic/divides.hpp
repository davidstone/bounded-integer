// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>

#include <bounded/detail/minmax.hpp>

namespace bounded {
namespace detail {

template<typename LHS, typename RHS>
constexpr auto divides_operator_range(LHS const & lhs, RHS const & rhs) noexcept {
	// If 1 falls within the range, that is the least positive divisor. The
	// other options are a range that are entirely positive, in which case I
	// want to return the least value, or the range is entirely negative or
	// zero, in which case I pick the greatest absolute value (which is the
	// minimum) so that the 'positive' divisor is not selected in a later step.
	// We can use similar logic for greatest_negative_divisor.
	auto const least_positive_divisor = (rhs.min <= 1 and 1 <= rhs.max) ? 1 : rhs.min;
	auto const greatest_negative_divisor = (rhs.min <= -1 and -1 <= rhs.max) ? -1 : rhs.max;

	auto const g0 = lhs.min / least_positive_divisor;
	auto const g1 = lhs.min / greatest_negative_divisor;
	auto const g2 = lhs.max / least_positive_divisor;
	auto const g3 = lhs.max / greatest_negative_divisor;

	auto const l0 = lhs.min / rhs.min;
	auto const l1 = lhs.min / rhs.max;
	auto const l2 = lhs.max / rhs.min;
	auto const l3 = lhs.max / rhs.max;

	return min_max(
		min(l0, l1, l2, l3, g0, g1, g2, g3),
		max(g0, g1, g2, g3)
	);
}

}	// namespace detail

BOUNDED_INTEGER_OPERATOR_OVERLOADS(/, divides_operator_range)

}	// namespace bounded

