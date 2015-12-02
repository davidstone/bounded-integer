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

#include "compound_assignment.hpp"
#include "../noexcept.hpp"

#include <type_traits>

namespace bounded {
namespace detail {
namespace arithmetic {

template<typename T>
constexpr auto operator++(T & value) BOUNDED_NOEXCEPT_DECLTYPE(
	value += constant<1>
)

template<typename T>
constexpr auto operator++(T & value, int) noexcept(std::is_nothrow_copy_constructible<T>::value and std::is_nothrow_move_constructible<T>::value and noexcept(++value)) {
	auto previous = value;
	++value;
	return previous;
}


template<typename T>
constexpr auto operator--(T & value) BOUNDED_NOEXCEPT_DECLTYPE(
	value -= constant<1>
)

template<typename T>
constexpr auto operator--(T & value, int) noexcept(std::is_nothrow_copy_constructible<T>::value and std::is_nothrow_move_constructible<T>::value and noexcept(--value)) {
	auto previous = value;
	--value;
	return previous;
}

#define BOUNDED_COMMON_ARITHMETIC \
	using bounded::detail::arithmetic::operator++; \
	using bounded::detail::arithmetic::operator--;

}	// namespace arithmetic

BOUNDED_COMMON_ARITHMETIC

}	// namespace detail

BOUNDED_COMMON_ARITHMETIC

}	// namespace bounded
