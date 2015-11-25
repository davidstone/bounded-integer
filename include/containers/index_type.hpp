// A type suitable to index into a container / iterator
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

#include <bounded_integer/bounded_integer.hpp>

#include <limits>
#include <type_traits>

namespace containers {
namespace detail {

template<typename T>
constexpr auto index_type_source() -> typename T::size_type {
	return std::numeric_limits<typename T::size_type>::min();
}
template<typename T>
constexpr auto index_type_source() -> typename T::difference_type {
	return std::numeric_limits<typename T::difference_type>::min();
}

}	// namespace detail

template<typename T>
using index_type = bounded::checked_integer<
	0,
	static_cast<std::intmax_t>(std::numeric_limits<decltype(detail::index_type_source<std::decay_t<T>>())>::max()) - 1,
	std::out_of_range
>;

}	// namespace containers
