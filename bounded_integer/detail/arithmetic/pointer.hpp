// Pointer arithmetic
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

#include "common.hpp"

#include "../is_bounded_integer.hpp"
#include "../requires.hpp"

namespace bounded {

template<typename T, typename Integer, BOUNDED_REQUIRES(is_bounded_integer<Integer>)>
constexpr auto operator+(T * const pointer, Integer const & number) noexcept {
	return pointer + number.value();
}

template<typename Integer, typename T, BOUNDED_REQUIRES(is_bounded_integer<Integer>)>
constexpr auto operator+(Integer const & number, T * const pointer) noexcept {
	return number.value() + pointer;
}

// Not possible to overload operator[]. See
// https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/CmBERU_sr8Y

}	// namespace bounded

