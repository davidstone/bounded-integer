// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/algorithms/binary_search.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/is_range.hpp>
#include <containers/legacy_iterator.hpp>
#include <containers/range_view.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <algorithm>

namespace containers {

template<iterator ForwardIterator>
constexpr auto rotate(ForwardIterator first, ForwardIterator middle, sentinel_for<ForwardIterator> auto const last) {
	if (first == middle) {
		return last;
	}
	if (middle == last) {
		return first;
	}

	auto next_middle = first;

	for (; middle != last; ++middle) {
		if (first == next_middle) {
			next_middle = middle;
		}
		::containers::swap(*first, *middle);
		++first;
	}

	::containers::rotate(first, next_middle, last);
	return first;
}

namespace detail {

constexpr auto insertion_sort = [](auto const first, auto const last, auto cmp) {
	for (auto it = first; it != last; ++it) {
		auto const insertion = upper_bound(range_view(first, it), *it, cmp);
		::containers::rotate(insertion, it, ::containers::next(it));
	}
};

} // namespace detail

// TODO: Implement something like ska_sort
// This is currently highly suboptimal at compile time, since it is an
// implementation of O(n^2) insertion sort. When the standard library has been
// updated to C++20, std::sort will be constexpr.
constexpr inline struct sort_t {
	template<iterator Iterator, typename Compare>
	constexpr void operator()(Iterator const first, sentinel_for<Iterator> auto const last, Compare cmp) const {
		// Temporary, until std::sort is constexpr
		if (std::is_constant_evaluated()) {
			detail::insertion_sort(first, last, cmp);
		} else {
			std::sort(make_legacy_iterator(first), make_legacy_iterator(last), cmp);
		}
	}
	template<range Range, typename Compare>
	constexpr void operator()(Range & range, Compare cmp) const {
		operator()(begin(range), end(range), cmp);
	}
	template<range Range>
	constexpr void operator()(Range & range) const {
		operator()(range, std::less{});
	}
} sort;

constexpr inline struct is_sorted_t {
	template<range Range, typename Compare>
	constexpr bool operator()(Range && range, Compare cmp) const {
		auto first = begin(range);
		auto const last = end(range);
		if (first != last) {
			for (auto next = containers::next(first); next != last; ++next) {
				if (cmp(*next, *first)) {
					return false;
				}
				first = next;
			}
		}
		return true;
	}
	template<range Range>
	constexpr bool operator()(Range && range) const {
		return operator()(range, std::less{});
	}
} is_sorted;


}	// namespace containers
