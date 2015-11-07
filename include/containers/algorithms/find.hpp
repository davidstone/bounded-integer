// find that works with a sentinel value
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

#include <type_traits>

namespace containers {

template<typename InputIterator, typename Sentinel, typename UnaryPredicate>
constexpr auto find_if(InputIterator first, Sentinel const last, UnaryPredicate p) noexcept(noexcept(first != last) and noexcept(++first) and noexcept(p(*first)) and std::is_nothrow_move_constructible<InputIterator>::value) {
	for (; first != last; ++first) {
		if (p(*first)) {
			break;
		}
	}
	return first;
}

template<typename InputIterator, typename Sentinel, typename T>
constexpr auto find(InputIterator first, Sentinel const last, T const & value) {
	return find_if(first, last, [&](auto const & other) { return other == value; });
}

}	// namespace containers
