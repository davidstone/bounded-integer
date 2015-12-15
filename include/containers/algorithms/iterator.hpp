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
#include <bounded_integer/integer_range.hpp>

#include <iterator>

namespace containers {
namespace detail {

template<typename Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset, std::random_access_iterator_tag) BOUNDED_NOEXCEPT(
	static_cast<void>(it += offset)
)
template<typename Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset, std::bidirectional_iterator_tag) {
	for (auto const n : bounded::integer_range(bounded::abs(offset))) {
		static_cast<void>(n);
		if (offset >= bounded::constant<0>) {
			++it;
		} else {
			--it;
		}
	}
}
template<typename Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset, std::forward_iterator_tag) {
	for (auto const n : bounded::integer_range(offset)) {
		static_cast<void>(n);
		++it;
	}
}


}	// namespace detail

template<typename Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset) BOUNDED_NOEXCEPT(
	::containers::detail::advance(it, offset, typename std::iterator_traits<Iterator>::iterator_category{})
)

namespace detail {

template<
	typename Iterator,
	BOUNDED_REQUIRES(bounded::is_bounded_integer<typename std::iterator_traits<Iterator>::difference_type>)
>
constexpr auto iterator_one() noexcept {
	return bounded::constant<1>;
}

template<
	typename Iterator,
	BOUNDED_REQUIRES(!bounded::is_bounded_integer<typename std::iterator_traits<Iterator>::difference_type>)
>
constexpr auto iterator_one() noexcept {
	return 1;
}

}	// namespace detail

template<
	typename Iterator,
	typename Offset = decltype(::containers::detail::iterator_one<Iterator>()),
	BOUNDED_REQUIRES(is_iterator<Iterator>)
>
constexpr auto next(Iterator it, Offset const offset = ::containers::detail::iterator_one<Iterator>()) {
	::containers::advance(it, offset);
	return it;
}


template<
	typename Iterator,
	typename Offset = decltype(::containers::detail::iterator_one<Iterator>()),
	BOUNDED_REQUIRES(
		is_iterator<Iterator> and
		bounded::is_bounded_integer<typename std::iterator_traits<Iterator>::difference_type>
	)
>
constexpr auto prev(Iterator it, Offset const offset = ::containers::detail::iterator_one<Iterator>()) {
	::containers::advance(it, -offset);
	return it;
}

}	// namespace containers
