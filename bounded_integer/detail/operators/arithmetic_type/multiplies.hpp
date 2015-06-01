// result_type of multiplies
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

struct multiplies {
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const noexcept {
		return std::forward<LHS>(lhs) * std::forward<RHS>(rhs);
	}
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
struct operator_range<lhs_min, lhs_max, rhs_min, rhs_max, multiplies> {
private:
	static constexpr intmax_t p0 = lhs_min * rhs_min;
	static constexpr intmax_t p1 = lhs_min * rhs_max;
	static constexpr intmax_t p2 = lhs_max * rhs_min;
	static constexpr intmax_t p3 = lhs_max * rhs_max;
public:
	static constexpr auto min() noexcept -> intmax_t {
		return ::bounded::min(p0, p1, p2, p3);
	}
	static constexpr auto max() noexcept -> intmax_t {
		return ::bounded::max(p0, p1, p2, p3);
	}
	static_assert(min() <= max(), "Range is inverted.");
};

}	// namespace detail
}	// namespace bounded

