// Bitwise and operator overload when each argument is a bounded::integer
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

// I would prefer not to implement this, as bounded::integer is not a collection
// of bits. However, many people use x & (y - 1) when they mean x % y, when y is
// a power of two. For instance, gcc's implementation of std::sort

namespace bounded {
namespace detail {

// Assume always positive integers for now
// Not the tightest possible bounds, but probably good enough for now
template<typename LHS, typename RHS>
constexpr auto bitwise_and_operator_range(LHS const & lhs, RHS const & rhs) noexcept {
	return min_max(
		0,
		min(lhs.max, rhs.max)
	);
}

}	// namespace detail

BOUNDED_INTEGER_OPERATOR_OVERLOADS(&, detail::bitwise_and_operator_range)

}	// namespace bounded

