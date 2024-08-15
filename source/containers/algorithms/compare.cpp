// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.compare;

import containers.begin_end;
import containers.iterator;
import containers.range;
import containers.sentinel_for;
import containers.size;
import containers.sized_range;

import bounded;
import std_module;

namespace containers {

export template<iterator InputIterator1, iterator InputIterator2>
constexpr auto lexicographical_compare_3way(InputIterator1 first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 first2, sentinel_for<InputIterator2> auto const last2, auto cmp) -> decltype(cmp(*first1, *first2)) {
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

export constexpr auto lexicographical_compare_3way(range auto && range1, range auto && range2, auto cmp) {
	return ::containers::lexicographical_compare_3way(
		containers::begin(OPERATORS_FORWARD(range1)),
		containers::end(OPERATORS_FORWARD(range1)),
		containers::begin(OPERATORS_FORWARD(range2)),
		containers::end(OPERATORS_FORWARD(range2)),
		std::move(cmp)
	);
}


export template<iterator InputIterator1, iterator InputIterator2>
constexpr auto lexicographical_compare_3way(InputIterator1 first1, sentinel_for<InputIterator1> auto last1, InputIterator2 first2, sentinel_for<InputIterator2> auto last2) {
	return ::containers::lexicographical_compare_3way(
		std::move(first1),
		std::move(last1),
		std::move(first2),
		std::move(last2),
		std::compare_three_way()
	);
}

export constexpr auto lexicographical_compare_3way(range auto && range1, range auto && range2) {
	return ::containers::lexicographical_compare_3way(
		OPERATORS_FORWARD(range1),
		OPERATORS_FORWARD(range2),
		std::compare_three_way()
	);
}


export template<iterator InputIterator1, iterator InputIterator2>
constexpr auto lexicographical_compare_3way(InputIterator1 first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 first2, auto cmp) requires(!sentinel_for<decltype(cmp), InputIterator2>) {
	for (; first1 != last1; ++first1, ++first2) {
		if (auto const result = cmp(*first1, *first2); result != 0) {
			return result;
		}
	}
	return std::strong_ordering::equal;
}

export template<iterator InputIterator1>
constexpr auto lexicographical_compare_3way(InputIterator1 first1, sentinel_for<InputIterator1> auto last1, iterator auto first2) {
	return ::containers::lexicographical_compare_3way(
		std::move(first1),
		std::move(last1),
		std::move(first2),
		std::compare_three_way()
	);
}



// Shorter ranges compare less than longer ranges. Ranges of the same length
// compare lexicographically.
export constexpr auto shortlex_compare(sized_range auto && range1, sized_range auto && range2, auto cmp) {
	if (auto const result = ::containers::size(range1) <=> ::containers::size(range2); result != 0) {
		return result;
	}
	return ::containers::lexicographical_compare_3way(
		::containers::begin(OPERATORS_FORWARD(range1)),
		::containers::end(OPERATORS_FORWARD(range1)),
		::containers::begin(OPERATORS_FORWARD(range2)),
		std::move(cmp)
	);
}

export constexpr auto shortlex_compare(sized_range auto && range1, sized_range auto && range2) {
	return ::containers::shortlex_compare(
		OPERATORS_FORWARD(range1),
		OPERATORS_FORWARD(range2),
		std::compare_three_way()
	);
}









export template<iterator InputIterator1, iterator InputIterator2>
constexpr auto equal(InputIterator1 first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 first2, sentinel_for<InputIterator2> auto const last2, auto cmp) {
	for (; first1 != last1 and first2 != last2; ++first1, ++first2) {
		if (!cmp(*first1, *first2)) {
			return false;
		}
	}
	return first1 == last1 and first2 == last2;
}


export template<iterator InputIterator1, iterator InputIterator2>
constexpr auto equal(InputIterator1 first1, sentinel_for<InputIterator1> auto last1, InputIterator2 first2, sentinel_for<InputIterator2> auto last2) {
	return ::containers::equal(
		std::move(first1),
		std::move(last1),
		std::move(first2),
		std::move(last2),
		bounded::equal_to()
	);
}

export template<iterator InputIterator1, iterator InputIterator2>
constexpr auto equal(InputIterator1 first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 first2, auto cmp) requires(!sentinel_for<decltype(cmp), InputIterator2>) {
	for (; first1 != last1; ++first1, ++first2) {
		if (!cmp(*first1, *first2)) {
			return false;
		}
	}
	return true;
}

export template<iterator InputIterator1, iterator InputIterator2>
constexpr auto equal(InputIterator1 first1, sentinel_for<InputIterator1> auto last1, InputIterator2 first2) {
	return ::containers::equal(
		std::move(first1),
		std::move(last1),
		std::move(first2),
		bounded::equal_to()
	);
}

export template<range Range1, range Range2>
constexpr auto equal(Range1 && range1, Range2 && range2, auto cmp) {
	if constexpr (sized_range<Range1> and sized_range<Range2>) {
		return
			::containers::size(range1) == ::containers::size(range2) and
			::containers::equal(
				containers::begin(OPERATORS_FORWARD(range1)),
				containers::end(OPERATORS_FORWARD(range1)),
				containers::begin(OPERATORS_FORWARD(range2)),
				std::move(cmp)
			);
	} else {
		return ::containers::equal(
			containers::begin(OPERATORS_FORWARD(range1)),
			containers::end(OPERATORS_FORWARD(range1)),
			containers::begin(OPERATORS_FORWARD(range2)),
			containers::end(OPERATORS_FORWARD(range2)),
			std::move(cmp)
		);
	}
}

export template<range Range1, range Range2>
constexpr auto equal(Range1 && range1, Range2 && range2) {
	return ::containers::equal(
		OPERATORS_FORWARD(range1),
		OPERATORS_FORWARD(range2),
		bounded::equal_to()
	);
}

} // namespace containers
