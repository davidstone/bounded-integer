// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_iterator.hpp>

#include <bounded/integer.hpp>
#include <bounded/integer_range.hpp>

#include <iterator>

namespace containers {

// TODO: noexcept

using namespace bounded::literal;

namespace detail {

template<typename Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset, std::random_access_iterator_tag) BOUNDED_NOEXCEPT(
	static_cast<void>(it += offset)
)
template<typename Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset, std::bidirectional_iterator_tag) {
	for (auto const n : bounded::integer_range(bounded::abs(offset))) {
		static_cast<void>(n);
		if (offset >= 0_bi) {
			++it;
		} else {
			--it;
		}
	}
}
template<typename Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset, std::input_iterator_tag) {
	for (auto const n : bounded::integer_range(offset)) {
		static_cast<void>(n);
		++it;
	}
}


}	// namespace detail

template<typename Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset) BOUNDED_NOEXCEPT_VALUE(
	::containers::detail::advance(it, offset, typename std::iterator_traits<Iterator>::iterator_category{})
)

namespace detail {

template<
	typename Iterator,
	BOUNDED_REQUIRES(bounded::is_bounded_integer<typename std::iterator_traits<Iterator>::difference_type>)
>
constexpr auto iterator_one() noexcept {
	return 1_bi;
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
	BOUNDED_REQUIRES(is_iterator<Iterator>)
>
constexpr auto prev(Iterator it, Offset const offset = ::containers::detail::iterator_one<Iterator>()) {
	::containers::advance(it, -offset);
	return it;
}

}	// namespace containers
