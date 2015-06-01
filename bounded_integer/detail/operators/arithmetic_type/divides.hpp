// result_type of divides
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

#include "../../minmax.hpp"

#include <utility>

namespace bounded {
namespace detail {

struct divides {
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const noexcept {
		static_assert(noexcept(std::forward<LHS>(lhs) / std::forward<RHS>(rhs)), "Division can throw exceptions."); 
		return std::forward<LHS>(lhs) / std::forward<RHS>(rhs);
	}
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
struct operator_range<lhs_min, lhs_max, rhs_min, rhs_max, divides> {
private:
	static_assert(rhs_min != 0 or rhs_max != 0, "division is not defined for a divisor of zero.");
	// If 1 falls within the range, that is the least positive divisor. The
	// other options are a range that are entirely positive, in which case I
	// want to return the least value, or the range is entirely negative or
	// zero, in which case I pick the greatest absolute value (which is the
	// minimum) so that the 'positive' divisor is not selected in a later step.
	// We can use similar logic for greatest_negative_divisor.
	static constexpr intmax_t least_positive_divisor = (rhs_min <= 1 and 1 <= rhs_max) ? 1 : rhs_min;
	static constexpr intmax_t greatest_negative_divisor = (rhs_min <= -1 and -1 <= rhs_max) ? -1 : rhs_max;
	static constexpr intmax_t g0 = lhs_min / least_positive_divisor;
	static constexpr intmax_t g1 = lhs_min / greatest_negative_divisor;
	static constexpr intmax_t g2 = lhs_max / least_positive_divisor;
	static constexpr intmax_t g3 = lhs_max / greatest_negative_divisor;

public:
	static constexpr auto min() noexcept -> intmax_t {
		constexpr auto l0 = lhs_min / rhs_min;
		constexpr auto l1 = lhs_min / rhs_max;
		constexpr auto l2 = lhs_max / rhs_min;
		constexpr auto l3 = lhs_max / rhs_max;
		return ::bounded::min(l0, l1, l2, l3, g0, g1, g2, g3);
	}
	static constexpr auto max() noexcept -> intmax_t {
		return ::bounded::max(g0, g1, g2, g3);
	}
	static_assert(min() <= max(), "Range is inverted.");
};


}	// namespace detail
}	// namespace bounded

