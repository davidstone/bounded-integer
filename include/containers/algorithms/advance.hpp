// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_iterator.hpp>

#include <bounded/integer.hpp>

#include <iterator>

namespace containers {

using namespace bounded::literal;

namespace detail {

template<iterator Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset, std::random_access_iterator_tag) {
	it += offset.value();
}
template<iterator Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset, std::random_access_iterator_tag) requires requires { it += offset; } {
	it += offset;
}

template<iterator Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset, std::bidirectional_iterator_tag) {
	using counter = std::common_type_t<decltype(bounded::abs(offset)), bounded::constant_t<0>>;
	for (auto n = counter(0_bi); n != bounded::abs(offset); ++n) {
		if (offset >= 0_bi) {
			++it;
		} else {
			--it;
		}
	}
}
template<iterator Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset, std::input_iterator_tag) {
	using counter = std::common_type_t<Offset, bounded::constant_t<0>>;
	for (auto n = counter(0_bi); n != offset; ++n) {
		++it;
	}
}


}	// namespace detail

template<iterator Iterator, typename Offset>
constexpr auto advance(Iterator & it, Offset const offset) {
	return ::containers::detail::advance(it, offset, typename std::iterator_traits<Iterator>::iterator_category{});
}

namespace detail {

template<iterator Iterator>
constexpr auto iterator_one() {
	if constexpr (bounded::bounded_integer<typename std::iterator_traits<Iterator>::difference_type>) {
		return 1_bi;
	} else {
		return 1;
	}
}

}	// namespace detail

template<iterator Iterator, typename Offset = decltype(::containers::detail::iterator_one<Iterator>())>
constexpr auto next(Iterator it, Offset const offset = ::containers::detail::iterator_one<Iterator>()) {
	::containers::advance(it, offset);
	return it;
}


template<iterator Iterator, typename Offset = decltype(::containers::detail::iterator_one<Iterator>())>
constexpr auto prev(Iterator it, Offset const offset = ::containers::detail::iterator_one<Iterator>()) {
	::containers::advance(it, -offset);
	return it;
}

}	// namespace containers
