// result_type of minus
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

struct minus {
	template<typename LHS, typename RHS>
	constexpr auto operator()(LHS && lhs, RHS && rhs) const noexcept {
		return std::forward<LHS>(lhs) - std::forward<RHS>(rhs);
	}
};

constexpr auto operator_range(min_max lhs, min_max rhs, minus) noexcept {
	return min_max(
		lhs.min - rhs.max,
		lhs.max - rhs.min
	);
}

}	// namespace detail
}	// namespace bounded

