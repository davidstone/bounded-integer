// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.compare;

import containers.c_array;
import containers.begin_end;
import containers.empty_range;
import containers.is_iterator;
import containers.is_iterator_sentinel;
import containers.range;
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
	return ::containers::lexicographical_compare_3way(containers::begin(range1), containers::end(range1), containers::begin(range2), containers::end(range2), std::move(cmp));
}


export template<iterator InputIterator1, iterator InputIterator2>
constexpr auto lexicographical_compare_3way(InputIterator1 const first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 const first2, sentinel_for<InputIterator2> auto const last2) {
	return ::containers::lexicographical_compare_3way(first1, last1, first2, last2, std::compare_three_way());
}

export constexpr auto lexicographical_compare_3way(range auto && range1, range auto && range2) {
	return ::containers::lexicographical_compare_3way(range1, range2, std::compare_three_way());
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
constexpr auto lexicographical_compare_3way(InputIterator1 const first1, sentinel_for<InputIterator1> auto const last1, iterator auto const first2) {
	return ::containers::lexicographical_compare_3way(first1, last1, first2, std::compare_three_way());
}



// Shorter ranges compare less than longer ranges. Ranges of the same length
// compare lexicographically.
export constexpr auto shortlex_compare(sized_range auto && range1, sized_range auto && range2, auto cmp) {
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

export constexpr auto shortlex_compare(sized_range auto && range1, sized_range auto && range2) {
	return ::containers::shortlex_compare(range1, range2, std::compare_three_way());
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
constexpr auto equal(InputIterator1 const first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 const first2, sentinel_for<InputIterator2> auto const last2) {
	return ::containers::equal(first1, last1, first2, last2, bounded::equal_to());
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
constexpr auto equal(InputIterator1 const first1, sentinel_for<InputIterator1> auto const last1, InputIterator2 const first2) {
	return ::containers::equal(first1, last1, first2, bounded::equal_to());
}

export template<range Range1, range Range2>
constexpr auto equal(Range1 && range1, Range2 && range2, auto cmp) {
	if constexpr (sized_range<Range1> and sized_range<Range2>) {
		return
			::containers::size(range1) == ::containers::size(range2) and
			::containers::equal(containers::begin(range1), containers::end(range1), containers::begin(range2), std::move(cmp));
	} else {
		return ::containers::equal(containers::begin(range1), containers::end(range1), containers::begin(range2), containers::end(range2), std::move(cmp));
	}
}

export template<range Range1, range Range2>
constexpr auto equal(Range1 && range1, Range2 && range2) {
	return ::containers::equal(range1, range2, bounded::equal_to());
}

} // namespace containers

constexpr auto empty = containers::empty_range<int>();

static_assert(containers::lexicographical_compare_3way(empty, empty) == 0);
static_assert(containers::equal(empty, empty));

constexpr containers::c_array<int, 1> one_one = {1};
constexpr containers::c_array<int, 1> one_two = {2};

static_assert(containers::lexicographical_compare_3way(empty, one_one) < 0);
static_assert(containers::lexicographical_compare_3way(one_one, empty) > 0);
static_assert(!containers::equal(empty, one_one));
static_assert(!containers::equal(one_one, empty));

static_assert(containers::lexicographical_compare_3way(one_one, one_two) < 0);
static_assert(containers::lexicographical_compare_3way(one_two, one_one) > 0);
static_assert(!containers::equal(one_one, one_two));
static_assert(!containers::equal(one_two, one_one));

static_assert(containers::equal(one_one, one_one));
static_assert(containers::equal(one_two, one_two));

constexpr containers::c_array<int, 2> two_forward = {1, 2};
constexpr containers::c_array<int, 2> two_reversed = {2, 1};

static_assert(containers::lexicographical_compare_3way(empty, two_forward) < 0);
static_assert(containers::lexicographical_compare_3way(two_forward, empty) > 0);
static_assert(!containers::equal(empty, two_forward));
static_assert(!containers::equal(two_forward, empty));

static_assert(containers::lexicographical_compare_3way(one_one, two_forward) < 0);
static_assert(containers::lexicographical_compare_3way(two_forward, one_one) > 0);
static_assert(!containers::equal(one_one, two_forward));
static_assert(!containers::equal(two_forward, one_one));

static_assert(containers::lexicographical_compare_3way(one_two, two_forward) > 0);
static_assert(containers::lexicographical_compare_3way(two_forward, one_two) < 0);
static_assert(!containers::equal(one_two, two_forward));
static_assert(!containers::equal(two_forward, one_two));

static_assert(containers::lexicographical_compare_3way(two_forward, two_reversed) < 0);
static_assert(containers::lexicographical_compare_3way(two_reversed, two_forward) > 0);
static_assert(!containers::equal(two_forward, two_reversed));
static_assert(!containers::equal(two_reversed, two_forward));

static_assert(containers::equal(two_forward, two_forward));
static_assert(containers::equal(two_reversed, two_reversed));

constexpr containers::c_array<int, 9> nine = {1, 2, 3, 4, 5, 6, 7, 8, 9};
constexpr containers::c_array<int, 10> ten = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

static_assert(containers::lexicographical_compare_3way(empty, nine) < 0);
static_assert(containers::lexicographical_compare_3way(nine, empty) > 0);
static_assert(!containers::equal(empty, nine));
static_assert(!containers::equal(nine, empty));

static_assert(containers::lexicographical_compare_3way(one_one, nine) < 0);
static_assert(containers::lexicographical_compare_3way(nine, one_one) > 0);
static_assert(!containers::equal(one_one, nine));
static_assert(!containers::equal(nine, one_one));

static_assert(containers::lexicographical_compare_3way(one_two, nine) > 0);
static_assert(containers::lexicographical_compare_3way(nine, one_two) < 0);
static_assert(!containers::equal(one_two, nine));
static_assert(!containers::equal(nine, one_two));

static_assert(containers::lexicographical_compare_3way(nine, two_reversed) < 0);
static_assert(containers::lexicographical_compare_3way(two_reversed, nine) > 0);
static_assert(!containers::equal(nine, two_reversed));
static_assert(!containers::equal(two_reversed, nine));

static_assert(containers::lexicographical_compare_3way(nine, ten) < 0);
static_assert(containers::lexicographical_compare_3way(ten, nine) > 0);
static_assert(!containers::equal(nine, ten));
static_assert(!containers::equal(ten, nine));

static_assert(containers::equal(nine, nine));
static_assert(containers::equal(ten, ten));
