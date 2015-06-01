// result_type of plus
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

#include <utility>

namespace bounded {
namespace detail {

struct plus {
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const noexcept {
		return std::forward<LHS>(lhs) + std::forward<RHS>(rhs);
	}
};

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
struct operator_range<lhs_min, lhs_max, rhs_min, rhs_max, plus> {
	static constexpr auto min() noexcept -> intmax_t {
		return lhs_min + rhs_min;
	}
	static constexpr auto max() noexcept -> intmax_t {
		return lhs_max + rhs_max;
	}
	static_assert(min() <= max(), "Range is inverted.");
};

}	// namespace detail
}	// namespace bounded

