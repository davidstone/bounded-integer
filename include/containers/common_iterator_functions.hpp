// Functions common to most iterators
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

#include <containers/common_functions.hpp>
#include <containers/is_iterator.hpp>

#include <bounded/integer.hpp>

// The strange namespacing and using declarations here are to ensure these
// functions are picked up by ADL for types defined in namespaces ::container or
// ::container::detail

namespace containers {
namespace detail {

// The value of a difference is equal to the largest possible size for
// std::distance(begin(), end()), and it is equal to the negation of that value
// for std::distance(end(), begin())
template<typename Size>
using basic_difference_type = std::common_type_t<
	decltype(-std::declval<Size>()),
	Size
>;

namespace common {

template<typename Offset, typename Iterator, BOUNDED_REQUIRES(std::numeric_limits<Offset>::is_integer and is_iterator<Iterator>)>
constexpr auto operator+(Offset const offset, Iterator const it) BOUNDED_NOEXCEPT(
	it + offset
)

}	// namespace common

using containers::detail::common::operator+;

}	// namespace detail

using containers::detail::common::operator+;

}	// namespace containers
