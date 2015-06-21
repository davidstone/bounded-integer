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

#include "../forward_declaration.hpp"

#include <cstdint>

namespace bounded {

template<typename T, intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
constexpr auto operator+(T * const pointer, integer<minimum, maximum, overflow_policy, storage> const & number) noexcept -> T * {
	return pointer + number.value();
}

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, typename T>
constexpr auto operator+(integer<minimum, maximum, overflow_policy, storage> const & number, T * const pointer) noexcept -> T * {
	return number.value() + pointer;
}

template<typename T, intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
constexpr auto operator-(T * const pointer, integer<minimum, maximum, overflow_policy, storage> const & number) noexcept -> T * {
	return pointer - number.value();
}


template<typename T, intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
decltype(auto) operator+=(T * & pointer, integer<minimum, maximum, overflow_policy, storage> const & number) noexcept {
	return pointer += number.value();
}

template<typename T, intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
decltype(auto) operator-=(T * & pointer, integer<minimum, maximum, overflow_policy, storage> const & number) noexcept {
	return pointer -= number.value();
}

// Not possible to overload operator[]. See
// https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/CmBERU_sr8Y

}	// namespace bounded

