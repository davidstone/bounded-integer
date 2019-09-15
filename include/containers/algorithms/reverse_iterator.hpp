// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_iterator.hpp>
#include <containers/iterator_adapter.hpp>

#include <bounded/detail/returns.hpp>

#include <iterator>

namespace containers {
namespace detail {

struct reverse_traits {
	template<typename BidirectionalIterator>
	static constexpr auto dereference(BidirectionalIterator it) BOUNDED_RETURNS(
		*::containers::prev(it)
	)

	template<typename RandomAccessIterator, typename Offset>
	static constexpr auto add(RandomAccessIterator it, Offset const offset) BOUNDED_RETURNS(
		it - offset
	)

	template<typename RandomAccessIterator>
	static constexpr auto subtract(RandomAccessIterator const lhs, RandomAccessIterator const rhs) BOUNDED_RETURNS(
		rhs - lhs
	)

	template<bounded::ordered RandomAccessIterator>
	static constexpr auto compare(RandomAccessIterator const lhs, RandomAccessIterator const rhs) {
		return compare(rhs, lhs);
	}
	template<typename RandomAccessIterator>
	static constexpr auto equal(RandomAccessIterator const lhs, RandomAccessIterator const rhs) {
		return rhs == lhs;
	}
};

}	// namespace detail

template<typename BidirectionalIterator>
constexpr auto reverse_iterator(BidirectionalIterator it) {
	return adapt_iterator(it, detail::reverse_traits{});
}


template<typename Iterable>
constexpr auto rbegin(Iterable && container) {
	return ::containers::reverse_iterator(end(BOUNDED_FORWARD(container)));
}
template<typename Iterable>
constexpr auto rend(Iterable && container) {
	return ::containers::reverse_iterator(begin(BOUNDED_FORWARD(container)));
}



template<typename Iterable>
constexpr auto crbegin(Iterable const & container) {
	return ::containers::reverse_iterator(end(container));
}
template<typename Iterable>
constexpr auto crend(Iterable const & container) {
	return ::containers::reverse_iterator(begin(container));
}


}	// namespace containers
