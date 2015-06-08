// Subtraction operator overload when each argument is a bounded::integer
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

#include <functional>
#include <utility>

namespace bounded {
namespace detail {

template<typename LHS, typename RHS>
constexpr auto operator_range(LHS const & lhs, RHS const & rhs, std::minus<>) noexcept {
	return min_max(
		lhs.min - rhs.max,
		lhs.max - rhs.min
	);
}

}	// namespace detail

BOUNDED_INTEGER_OPERATOR_OVERLOADS(-, std::minus<>)

}	// namespace bounded

