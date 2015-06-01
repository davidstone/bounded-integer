// result_type of left shift
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

#include <limits>
#include <utility>

namespace bounded {
namespace detail {

struct left_shift {
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const noexcept {
		static_assert(noexcept(std::forward<LHS>(lhs) << std::forward<RHS>(rhs)), "Left shift can throw exceptions."); 
		return std::forward<LHS>(lhs) << std::forward<RHS>(rhs);
	}
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
struct operator_range<lhs_min, lhs_max, rhs_min, rhs_max, left_shift> {
	static_assert(lhs_min >= 0 and rhs_min >= 0, "Left shift not defined for negative values.");
	static_assert(rhs_max <= std::numeric_limits<intmax_t>::digits, "Cannot left shift >= width of intmax_t.");
	static constexpr auto min() noexcept -> intmax_t {
		return lhs_min << rhs_min;
	}
	static constexpr auto max() noexcept -> intmax_t {
		return lhs_max << rhs_max;
	}
	static_assert(min() <= max(), "Range is inverted.");
};

}	// namespace detail
}	// namespace bounded

