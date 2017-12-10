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

#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {

template<typename InputIterator, typename Sentinel, typename UnaryPredicate>
constexpr auto all_of(InputIterator const first, Sentinel const last, UnaryPredicate p) BOUNDED_NOEXCEPT_VALUE(
	::containers::find_if_not(first, last, p) == last
)

template<typename InputIterator, typename Sentinel, typename UnaryPredicate>
constexpr auto any_of(InputIterator const first, Sentinel const last, UnaryPredicate p) BOUNDED_NOEXCEPT_VALUE(
	::containers::find_if(first, last, p) != last
)

template<typename InputIterator, typename Sentinel, typename UnaryPredicate>
constexpr auto none_of(InputIterator const first, Sentinel const last, UnaryPredicate p) BOUNDED_NOEXCEPT_VALUE(
	::containers::find_if(first, last, p) == last
)

}	// namespace containers
