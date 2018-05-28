// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_iterator.hpp>
#include <containers/iterator_adapter.hpp>

#include <iterator>

namespace containers {
namespace detail {

struct reverse_dereference {
	template<typename BidirectionalIterator>
	constexpr auto operator()(BidirectionalIterator it) const BOUNDED_NOEXCEPT_DECLTYPE(
		*std::prev(it)
	)
};

struct reverse_add {
	template<typename RandomAccessIterator, typename Offset>
	constexpr auto operator()(RandomAccessIterator it, Offset const offset) const BOUNDED_NOEXCEPT_DECLTYPE(
		it - offset
	)
};

struct reverse_subtract {
	template<typename RandomAccessIterator>
	constexpr auto operator()(RandomAccessIterator const lhs, RandomAccessIterator const rhs) const BOUNDED_NOEXCEPT_DECLTYPE(
		rhs - lhs
	)
};

struct reverse_less {
	template<typename RandomAccessIterator>
	constexpr auto operator()(RandomAccessIterator const lhs, RandomAccessIterator const rhs) const BOUNDED_NOEXCEPT_DECLTYPE(
		rhs < lhs
	)
};

}	// namespace detail

template<typename BidirectionalIterator>
constexpr auto reverse_iterator(BidirectionalIterator it) BOUNDED_NOEXCEPT_VALUE(
	::containers::iterator_adapter(it, detail::reverse_dereference{}, detail::reverse_add{}, detail::reverse_subtract{}, detail::reverse_less{})
)

template<typename BidirectionalIterator>
using reverse_iterator_t = decltype(reverse_iterator(std::declval<BidirectionalIterator>()));

}	// namespace containers
