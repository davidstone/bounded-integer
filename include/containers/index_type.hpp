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

template<typename T, bool container = is_container<T>>
struct index_type_source_c;

template<typename Container>
struct index_type_source_c<Container, true> {
	using type = typename Container::size_type;
};

template<typename Iterator>
struct index_type_source_c<Iterator, false> {
	static_assert(is_iterator<Iterator>);
	using type = typename Iterator::difference_type;
};

template<typename T>
using index_type_source = typename index_type_source_c<std::decay_t<T>>::type;

}	// namespace detail

template<typename T>
using index_type = bounded::checked_integer<
	0,
	static_cast<std::intmax_t>(std::numeric_limits<detail::index_type_source<T>>::max()) - 1,
	std::out_of_range
>;

}	// namespace containers
