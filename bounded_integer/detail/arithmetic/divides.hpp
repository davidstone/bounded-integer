// Division operator overload when each argument is a bounded::integer
// Copyright (C) 2015 David Stone
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

#pragma once

#include "base.hpp"

#include "../minmax.hpp"

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

