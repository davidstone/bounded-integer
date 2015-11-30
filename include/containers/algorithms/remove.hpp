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

#include <containers/algorithms/find.hpp>
#include <containers/algorithms/iterator.hpp>

#include <bounded_integer/bounded_integer.hpp>

#include <utility>

namespace containers {

template<typename ForwardIterator, typename Sentinel, typename Predicate>
constexpr auto remove_if(ForwardIterator const first, Sentinel const last, Predicate predicate) {
	auto new_last = ::containers::find_if(first, last, predicate);
	if (new_last != last) {
		for (auto it = ::containers::next(new_last); it != last; ++it) {
			if (!predicate(*it)) {
				*new_last = std::move(*it);
				++new_last;
			}
		}
	}
	return new_last;
}

template<typename ForwardIterator, typename Sentinel, typename T>
constexpr auto remove(ForwardIterator const first, Sentinel const last, T const & value) BOUNDED_NOEXCEPT(
	remove_if(first, last, bounded::equal_to(value))
)


}	// namespace containers
