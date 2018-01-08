// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/negate.hpp>

#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {

template<typename InputIterator, typename Sentinel, typename UnaryPredicate>
constexpr auto find_if(InputIterator first, Sentinel const last, UnaryPredicate p) noexcept(noexcept(first != last) and noexcept(++first) and noexcept(p(*first)) and std::is_nothrow_move_constructible<InputIterator>::value) {
	for (; first != last; ++first) {
		if (p(*first)) {
			break;
		}
	}
	return first;
}

template<typename InputIterator, typename Sentinel, typename UnaryPredicate>
constexpr auto find_if_not(InputIterator const first, Sentinel const last, UnaryPredicate p) BOUNDED_NOEXCEPT(
	::containers::find_if(first, last, ::containers::negate(std::move(p)))
)

template<typename InputIterator, typename Sentinel, typename T>
constexpr auto find(InputIterator const first, Sentinel const last, T const & value) BOUNDED_NOEXCEPT(
	::containers::find_if(first, last, bounded::equal_to(value))
)

}	// namespace containers
