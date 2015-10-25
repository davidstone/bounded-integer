// Absolute value
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
