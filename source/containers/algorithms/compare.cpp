// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>
#include <operators/forward.hpp>

export module containers.algorithms.compare;

import containers.begin_end;
import containers.iterator;
import containers.range;
import containers.range_reference_t;
import containers.sentinel_for;
import containers.size;
import containers.sized_range;

import bounded;
import std_module;

namespace containers {

template<typename Range1, typename Range2>
using comparison_result = std::compare_three_way_result_t<
	containers::range_reference_t<Range1>,
	containers::range_reference_t<Range2>
>;

export template<range Range1, range Range2, typename Compare = std::compare_three_way>
constexpr auto lexicographical_compare_assume_same_size(
	Range1 && range1,
	Range2 && range2,
	Compare cmp = std::compare_three_way()
) -> comparison_result<Range1, Range2> {
	if constexpr (sized_range<Range1> and sized_range<Range2>) {
		BOUNDED_ASSERT(::containers::size(range1) == ::containers::size(range2));
	}
	auto const last1 = containers::end(OPERATORS_FORWARD(range1));
	auto first1 = containers::begin(OPERATORS_FORWARD(range1));
	auto first2 = containers::begin(OPERATORS_FORWARD(range2));
	for (; first1 != last1; ++first1, ++first2) {
		if (auto const result = cmp(*first1, *first2); result != 0) {
			return result;
		}
	}
	return std::strong_ordering::equal;
}

export template<range Range1, range Range2, typename Compare = std::compare_three_way>
constexpr auto lexicographical_compare(
	Range1 && range1,
	Range2 && range2,
	Compare cmp = std::compare_three_way()
) -> comparison_result<Range1, Range2> {
	auto const last1 = containers::end(OPERATORS_FORWARD(range1));
	auto first1 = containers::begin(OPERATORS_FORWARD(range1));
	auto const last2 = containers::end(OPERATORS_FORWARD(range2));
	auto first2 = containers::begin(OPERATORS_FORWARD(range2));
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

// Shorter ranges compare less than longer ranges. Ranges of the same length
// compare lexicographically.
export template<sized_range Range1, sized_range Range2, typename Compare = std::compare_three_way>
constexpr auto shortlex_compare(
	Range1 && range1,
	Range2 && range2,
	Compare cmp = std::compare_three_way()
) -> comparison_result<Range1, Range2> {
	if (auto const result = ::containers::size(range1) <=> ::containers::size(range2); result != 0) {
		return result;
	}
	return ::containers::lexicographical_compare_assume_same_size(
		OPERATORS_FORWARD(range1),
		OPERATORS_FORWARD(range2),
		std::move(cmp)
	);
}


export template<range Range1, range Range2, typename Compare = std::equal_to<>>
constexpr auto equal_assume_same_size(
	Range1 && range1,
	Range2 && range2,
	Compare cmp = std::equal_to()
) -> bool {
	if constexpr (sized_range<Range1> and sized_range<Range2>) {
		BOUNDED_ASSERT(::containers::size(range1) == ::containers::size(range2));
	}
	auto const last1 = containers::end(OPERATORS_FORWARD(range1));
	auto first1 = containers::begin(OPERATORS_FORWARD(range1));
	auto first2 = containers::begin(OPERATORS_FORWARD(range2));
	for (; first1 != last1; ++first1, ++first2) {
		if (!cmp(*first1, *first2)) {
			return false;
		}
	}
	return true;
}


export template<range Range1, range Range2, typename Compare = std::equal_to<>>
constexpr auto equal(
	Range1 && range1,
	Range2 && range2,
	Compare cmp = std::equal_to()
) -> bool {
	if constexpr (sized_range<Range1> and sized_range<Range2>) {
		return
			::containers::size(range1) == ::containers::size(range2) and
			::containers::equal_assume_same_size(
				OPERATORS_FORWARD(range1),
				OPERATORS_FORWARD(range2),
				std::move(cmp)
			);
	} else {
		auto const last1 = containers::end(OPERATORS_FORWARD(range1));
		auto first1 = containers::begin(OPERATORS_FORWARD(range1));
		auto const last2 = containers::end(OPERATORS_FORWARD(range2));
		auto first2 = containers::begin(OPERATORS_FORWARD(range2));
		for (; first1 != last1 and first2 != last2; ++first1, ++first2) {
			if (!cmp(*first1, *first2)) {
				return false;
			}
		}
		return first1 == last1 and first2 == last2;
	}
}

} // namespace containers
