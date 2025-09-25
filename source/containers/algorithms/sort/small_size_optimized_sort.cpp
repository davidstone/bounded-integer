// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.algorithms.sort.small_size_optimized_sort;

import containers.algorithms.sort.is_sorted;
import containers.algorithms.sort.sort_exactly_n;

import containers.begin_end;
import containers.range;
import containers.range_size_t;
import containers.size;

import bounded;
import numeric_traits;
import std_module;

namespace containers {

using namespace bounded::literal;

export template<range Range>
inline constexpr auto small_size_optimized_sort(Range && r, auto const compare, auto const sort_large_range) -> void {
	constexpr auto min_size = numeric_traits::min_value<range_size_t<Range>>;
	constexpr auto max_size = numeric_traits::max_value<range_size_t<Range>>;
	auto do_sort = [&](auto const count) {
		if constexpr (min_size <= count and count <= max_size) {
			::containers::sort_exactly_n(containers::begin(r), count, compare);
			BOUNDED_ASSERT(::containers::is_sorted(r, compare));
		} else {
			std::unreachable();
		}
	};
	switch (static_cast<std::size_t>(containers::size(r))) {
		case 0:
		case 1:
			return;
		case 2:
			do_sort(2_bi);
			return;
		case 3:
			do_sort(3_bi);
			return;
		case 4:
			do_sort(4_bi);
			return;
		case 5:
			do_sort(5_bi);
			return;
		default:
			if constexpr (max_size > 5_bi) {
				sort_large_range(r, compare);
				return;
			} else {
				std::unreachable();
			}
	}
}

} // namespace containers
