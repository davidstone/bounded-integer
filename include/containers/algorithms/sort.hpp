// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/is_range.hpp>
#include <containers/legacy_iterator.hpp>
#include <containers/range_view.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <algorithm>

namespace containers {

template<typename LHS, typename RHS>
constexpr void swap(LHS & lhs, RHS & rhs) {
	auto temp = std::move(lhs);
	lhs = std::move(rhs);
	rhs = std::move(temp);
}

// TODO: Implement something like ska_sort
// This is currently highly suboptimal at compile time, since it is an
// implementation of O(n^2) selection sort. When the standard library has been
// updated to C++20, std::sort will be constexpr.
constexpr inline struct sort_t {
	template<typename Iterator, typename Sentinel, typename Compare> requires is_iterator_sentinel<Iterator, Sentinel>
	constexpr void operator()(Iterator const first, Sentinel const last, Compare cmp) const {
		if (std::is_constant_evaluated()) {
			for (auto it = first; it != last; ++it) {
				auto const selection = std::min_element(it, last, cmp);
				::containers::swap(*selection, *it);
			}
		} else {
			std::sort(containers::legacy_iterator(first), containers::legacy_iterator(last), cmp);
		}
	}
	template<typename Range, typename Compare> requires is_range<Range>
	constexpr void operator()(Range & range, Compare cmp) const {
		operator()(begin(range), end(range), cmp);
	}
	template<typename Range> requires is_range<Range>
	constexpr void operator()(Range & range) const {
		operator()(range, std::less{});
	}
} sort;

constexpr inline struct is_sorted_t {
	template<typename Range, typename Compare> requires is_range<Range>
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
	template<typename Range> requires is_range<Range>
	constexpr bool operator()(Range && range) const {
		return operator()(range, std::less{});
	}
} is_sorted;

}	// namespace containers
