// Overloads for the standard library math functions.
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

#include "cast.hpp"
#include "forward_declaration.hpp"
#include "is_bounded_integer.hpp"
#include "minmax.hpp"
#include "requires.hpp"
#include "arithmetic/unary_minus.hpp"

namespace bounded {

template<typename Integer, BOUNDED_REQUIRES(is_bounded_integer<Integer>)>
constexpr auto abs(Integer const value) noexcept {
	// The 0 has to be there to restrict the range of possible values. Without
	// it, abs(integer<-7, 3>) would be [-3, 7] instead of [0, 7].
	return increase_min<0>(max(value, -value), non_check);
}

}	// namespace bounded

namespace std {
// This overload is required for std::sort to work on gcc with iterators whose
// difference_type is a bounded::integer. It is not guaranteed to continue to be
// supported. It returns a built-in integer that is the log2 of the number
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounded::storage_type storage>
constexpr auto __lg(bounded::integer<minimum, maximum, overflow_policy, storage> const value) noexcept {
	return __lg(value.value());
}

}	// namespace std
