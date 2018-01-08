// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

#include <iterator>

namespace containers {

using namespace bounded::literal;

namespace detail {

template<typename Iterator, typename Sentinel>
constexpr auto distance(Iterator const first, Sentinel const last, std::random_access_iterator_tag) BOUNDED_NOEXCEPT_VALUE(
	last - first
)

// TODO: noexcept
template<typename Iterator, typename Sentinel>
constexpr auto distance(Iterator first, Sentinel const last, std::input_iterator_tag) {
	auto difference = typename std::iterator_traits<Iterator>::difference_type(0_bi);
	for (; first != last; ++first) {
		++difference;
	}
	return difference;
}

}	// namespace detail

template<typename InputIterator, typename Sentinel>
constexpr auto distance(InputIterator first, Sentinel const last) BOUNDED_NOEXCEPT_VALUE(
	::containers::detail::distance(first, last, typename std::iterator_traits<InputIterator>::iterator_category{})
)


}	// namespace containers
