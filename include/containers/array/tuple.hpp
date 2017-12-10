// tuple interface for containers::array
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

#include <containers/array/array.hpp>

#include <bounded/integer.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace containers {

template<std::size_t index, typename T, std::size_t size>
constexpr auto && get(array<T, size> const & a) noexcept {
	return a[bounded::constant<index>];
}
template<std::size_t index, typename T, std::size_t size>
constexpr auto && get(array<T, size> & a) {
	return a[bounded::constant<index>];
}
template<std::size_t index, typename T, std::size_t size>
constexpr auto && get(array<T, size> && a) noexcept {
	return std::move(a[bounded::constant<index>]);
}

}	// namespace containers

// I am not sure yet if it is legal to specialize these classes.
#if 0
namespace std {

template<typename T, std::size_t size>
struct tuple_size<::containers::array<T, size>> : integral_constant<std::size_t, size> {};

template<std::size_t index, typename T, std::size_t size>
struct tuple_element<index, ::containers::array<T, size>> {
	using type = T;
};

}	// namespace std
#endif

