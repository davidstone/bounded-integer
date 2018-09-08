// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_iterator.hpp>
#include <containers/iterator_adapter.hpp>

#include <iterator>

namespace containers {
namespace detail {

struct reverse_traits {
	template<typename BidirectionalIterator>
	static constexpr auto dereference(BidirectionalIterator it) BOUNDED_NOEXCEPT_DECLTYPE(
		*std::prev(it)
	)

	template<typename RandomAccessIterator, typename Offset>
	static constexpr auto add(RandomAccessIterator it, Offset const offset) BOUNDED_NOEXCEPT_DECLTYPE(
		it - offset
	)

	template<typename RandomAccessIterator>
	static constexpr auto subtract(RandomAccessIterator const lhs, RandomAccessIterator const rhs) BOUNDED_NOEXCEPT_DECLTYPE(
		rhs - lhs
	)

	template<typename RandomAccessIterator>
	static constexpr auto compare(RandomAccessIterator const lhs, RandomAccessIterator const rhs) BOUNDED_NOEXCEPT_DECLTYPE(
		::containers::detail::compare_adl::indirect_compare(rhs, lhs)
	)
};

}	// namespace detail

template<typename BidirectionalIterator>
constexpr auto reverse_iterator(BidirectionalIterator it) BOUNDED_NOEXCEPT_VALUE(
	adapt_iterator(it, detail::reverse_traits{})
)

}	// namespace containers
