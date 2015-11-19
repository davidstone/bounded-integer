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

#include <containers/is_iterator.hpp>

#include <bounded_integer/bounded_integer.hpp>

#include <iterator>

namespace containers {

// TODO: constexpr when gcc supports it
template<
	typename Iterator,
	typename Offset = decltype(bounded::constant<1>),
	BOUNDED_REQUIRES(
		is_iterator<Iterator> and
		bounded::is_bounded_integer<typename std::iterator_traits<Iterator>::difference_type>
	)
>
auto next(Iterator it, Offset const offset = bounded::constant<1>) {
	std::advance(it, offset);
	return it;
}
template<
	typename Iterator,
	typename Offset = typename std::iterator_traits<Iterator>::difference_type,
	BOUNDED_REQUIRES(
		is_iterator<Iterator> and
		!bounded::is_bounded_integer<typename std::iterator_traits<Iterator>::difference_type>
	)
>
auto next(Iterator it, Offset const offset = 1) {
	std::advance(it, offset);
	return it;
}


template<
	typename Iterator,
	typename Offset = decltype(bounded::constant<1>),
	BOUNDED_REQUIRES(
		is_iterator<Iterator> and
		bounded::is_bounded_integer<typename std::iterator_traits<Iterator>::difference_type>
	)
>
auto prev(Iterator it, Offset const offset = bounded::constant<1>) {
	std::advance(it, -offset);
	return it;
}
template<
	typename Iterator,
	typename Offset = typename std::iterator_traits<Iterator>::difference_type,
	BOUNDED_REQUIRES(
		is_iterator<Iterator> and
		!bounded::is_bounded_integer<typename std::iterator_traits<Iterator>::difference_type>
	)
>
auto prev(Iterator it, Offset const offset = 1) {
	std::advance(it, -offset);
	return it;
}

}	// namespace containers
