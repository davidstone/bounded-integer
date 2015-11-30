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

#include <iterator>
#include <limits>
#include <utility>

namespace containers {

template<typename InputIterator, typename Sentinel, typename Predicate>
constexpr auto count_if(InputIterator first, Sentinel const last, Predicate predicate) {
	constexpr auto maximum = std::numeric_limits<typename std::iterator_traits<InputIterator>::difference_type>::max();
	bounded::integer<0, static_cast<std::intmax_t>(maximum)> sum = bounded::constant<0>;
	for (; first != last; ++first) {
		if (predicate(*first)) {
			++sum;
		}
	}
	return sum;
}

template<typename InputIterator, typename Sentinel, typename T>
constexpr auto count(InputIterator first, Sentinel last, T const & value) BOUNDED_NOEXCEPT(
	::containers::count_if(std::move(first), std::move(last), bounded::equal_to(value))
)

}	// namespace containers
