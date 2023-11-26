// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.algorithms.sort.sort;

import containers.algorithms.sort.is_sorted;
import containers.algorithms.sort.small_size_optimized_sort;
import containers.algorithms.sort.sort_exactly_3;
import containers.algorithms.sort.sort_exactly_5;

import containers.algorithms.advance;
import containers.algorithms.partition;

import containers.begin_end;
import containers.legacy_iterator;
import containers.range;
import containers.range_size_t;
import containers.range_view;
import containers.size;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace containers {

constexpr auto heap_sort(range auto && r, auto const compare) -> void {
	auto const first = containers::begin(r);
	auto const last = containers::end(r);
	std::make_heap(maybe_legacy_iterator(first), maybe_legacy_iterator(last), compare);
	std::sort_heap(maybe_legacy_iterator(first), maybe_legacy_iterator(last), compare);
}

constexpr auto introsort(range auto && r, auto const compare, auto depth) -> void;

template<bounded::bounded_integer Depth>
constexpr auto introsort_impl(Depth const depth) {
	return [=](range auto && r, auto const compare) {
		if (depth == 0_bi) {
			::containers::heap_sort(r, compare);
			return;
		}
		auto const length = containers::size(r);
		auto const first = containers::begin(r);
		auto const last = containers::end(r);
		auto median = first + length / 2_bi;
		auto const before_last = containers::prev(last);
		auto median_of = [&](auto... its) {
			::containers::sort_exactly_n_impl(*its..., compare);
		};
		if (length >= 1000_bi) {
			median_of(first, first + length / 4_bi, median, median + length / 4_bi, before_last);
		} else {
			static_assert(max_small_sort_size >= 3_bi);
			median_of(first, median, before_last);
		}
		median = iterator_partition(first, median, last, compare);
		auto next_depth = Depth(bounded::assume_in_range<Depth>(depth - 1_bi));
		::containers::introsort(range_view(first, median), compare, next_depth);
		::containers::introsort(range_view(median, last), compare, next_depth);
	};
}

inline constexpr auto introsort(range auto && r, auto const compare, auto depth) -> void {
	::containers::small_size_optimized_sort(r, compare, introsort_impl(depth));
}

struct new_sort_t {
	template<range Range>
	constexpr auto operator()(Range & to_sort, auto compare) const -> void {
		if constexpr (numeric_traits::max_value<range_size_t<Range>> >= 2_bi) {
			auto const size = bounded::integer(containers::size(to_sort));
			auto const depth = 2_bi * bounded::log(size, 2_bi);
			introsort(
				to_sort,
				compare,
				bounded::integer<0, bounded::builtin_max_value<decltype(depth)>>(depth)
			);
		}
		BOUNDED_ASSERT(is_sorted(to_sort, compare));
	}
	constexpr auto operator()(range auto & to_sort) const -> void {
		operator()(to_sort, std::less());
	}
};
export constexpr auto new_sort = new_sort_t();

struct sort_t {
	static constexpr auto operator()(range auto & to_sort, auto cmp) -> void {
		std::sort(
			maybe_legacy_iterator(containers::begin(to_sort)),
			maybe_legacy_iterator(containers::end(to_sort)),
			cmp
		);
	}
	static constexpr auto operator()(range auto & to_sort) -> void {
		operator()(to_sort, std::less());
	}
};
export constexpr auto sort = sort_t();

} // namespace containers
