// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

#include <utility>

namespace containers {

template<iterator InputIterator1, iterator InputIterator2>
constexpr auto lexicographical_compare_3way(InputIterator1 first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 first2, sentinel_for<InputIterator2> auto const last2, auto cmp) {
	for (; first1 != last1 and first2 != last2; ++first1, ++first2) {
		if (auto const result = cmp(*first1, *first2); result != 0) {
			return result;
		}
	}
	return
		(first1 != last1) ? std::strong_ordering::greater :
		(first2 != last2) ? std::strong_ordering::less :
		std::strong_ordering::equal;
}

constexpr auto lexicographical_compare_3way(range auto const & range1, range auto const & range2, auto cmp) {
	return ::containers::lexicographical_compare_3way(containers::begin(range1), containers::end(range1), containers::begin(range2), containers::end(range2), std::move(cmp));
}


template<iterator InputIterator1, iterator InputIterator2>
constexpr auto lexicographical_compare_3way(InputIterator1 const first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 const first2, sentinel_for<InputIterator2> auto const last2) {
	return ::containers::lexicographical_compare_3way(first1, last1, first2, last2, bounded::compare_3way());
}

constexpr auto lexicographical_compare_3way(range auto const & range1, range auto const & range2) {
	return ::containers::lexicographical_compare_3way(range1, range2, bounded::compare_3way());
}


template<iterator InputIterator1, iterator InputIterator2>
constexpr auto lexicographical_compare_3way(InputIterator1 first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 first2, auto cmp) requires(!sentinel_for<decltype(cmp), InputIterator2>) {
	for (; first1 != last1; ++first1, ++first2) {
		if (auto const result = cmp(*first1, *first2); result != 0) {
			return result;
		}
	}
	return std::strong_ordering::equal;
}

template<iterator InputIterator1>
constexpr auto lexicographical_compare_3way(InputIterator1 const first1, sentinel_for<InputIterator1> auto const last1, iterator auto const first2) {
	return ::containers::lexicographical_compare_3way(first1, last1, first2, bounded::compare_3way());
}



// Shorter ranges compare less than longer ranges. Ranges of the same length
// compare lexicographically.
constexpr auto shortlex_compare(sized_range auto const & range1, sized_range auto const & range2, auto cmp) {
	if (auto const result = ::containers::size(range1) <=> ::containers::size(range2); result != 0) {
		return result;
	}
	return ::containers::lexicographical_compare_3way(
		::containers::begin(range1),
		::containers::end(range1),
		::containers::begin(range2),
		std::move(cmp)
	);
}

constexpr auto shortlex_compare(sized_range auto const & range1, sized_range auto const & range2) {
	return ::containers::shortlex_compare(range1, range2, bounded::compare_3way());
}









template<iterator InputIterator1, iterator InputIterator2>
constexpr auto equal(InputIterator1 first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 first2, sentinel_for<InputIterator2> auto const last2, auto cmp) {
	for (; first1 != last1 and first2 != last2; ++first1, ++first2) {
		if (!cmp(*first1, *first2)) {
			return false;
		}
	}
	return first1 == last1 and first2 == last2;
}


template<iterator InputIterator1, iterator InputIterator2>
constexpr auto equal(InputIterator1 const first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 const first2, sentinel_for<InputIterator2> auto const last2) {
	return ::containers::equal(first1, last1, first2, last2, bounded::equal_to());
}

template<iterator InputIterator1, iterator InputIterator2>
constexpr auto equal(InputIterator1 first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 first2, auto cmp) requires(!sentinel_for<decltype(cmp), InputIterator2>) {
	for (; first1 != last1; ++first1, ++first2) {
		if (!cmp(*first1, *first2)) {
			return false;
		}
	}
	return first1 == last1;
}

template<iterator InputIterator1, iterator InputIterator2>
constexpr auto equal(InputIterator1 const first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 const first2) {
	return ::containers::equal(first1, last1, first2, bounded::equal_to());
}

template<range Range1, range Range2>
constexpr auto equal(Range1 const & range1, Range2 const & range2, auto cmp) {
	if constexpr (sized_range<Range1> and sized_range<Range2>) {
		return
			::containers::size(range1) == ::containers::size(range2) and
			::containers::equal(containers::begin(range1), containers::end(range1), containers::begin(range2), std::move(cmp));
	} else {
		return ::containers::equal(containers::begin(range1), containers::end(range1), containers::begin(range2), containers::end(range2), std::move(cmp));
	}
}

template<range Range1, range Range2>
constexpr auto equal(Range1 const & range1, Range2 const & range2) {
	return ::containers::equal(range1, range2, bounded::equal_to());
}


} // namespace containers
