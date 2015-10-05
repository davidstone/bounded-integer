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

#include <containers/is_container.hpp>
#include <containers/is_iterator.hpp>

#include <bounded_integer/bounded_integer.hpp>

#include <limits>
#include <type_traits>

namespace containers {
namespace detail {

template<typename Container, BOUNDED_REQUIRES(is_container<Container>)>
auto index_type_source_impl(Container const &) -> typename Container::size_type;

template<typename Iterator, BOUNDED_REQUIRES(is_iterator<Iterator>)>
auto index_type_source_impl(Iterator const &) -> typename Iterator::difference_type;

template<typename T>
using index_type_source = decltype(index_type_source_impl(std::declval<T>()));

}	// namespace detail

template<typename T>
using index_type = bounded::checked_integer<
	0,
	static_cast<std::intmax_t>(std::numeric_limits<detail::index_type_source<T>>::max()) - 1,
	std::out_of_range
>;

}	// namespace containers