// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// This is an implementation of ska_sort, as described in
// https://probablydance.com/2016/12/27/i-wrote-a-faster-sorting-algorithm/ and
// https://probablydance.com/2017/01/17/faster-sorting-algorithm-part-2/
// https://github.com/skarupke/ska_sort is the original implementation.

export module containers.algorithms.sort.ska_sort;

import containers.algorithms.sort.inplace_radix_sort;
import containers.algorithms.sort.to_radix_sort_key;

import containers.algorithms.erase;
import containers.algorithms.unique;

import containers.begin_end;
import containers.is_range;
import containers.range_view;

namespace containers {

struct ska_sort_t {
	static constexpr void operator()(range auto && to_sort, auto const & extract_key) {
		::containers::inplace_radix_sort<128, 1024>(
			range_view(
				containers::begin(to_sort),
				containers::end(to_sort)
			),
			extract_key
		);
	}
	static constexpr void operator()(range auto && to_sort) {
		operator()(to_sort, to_radix_sort_key);
	}
};
export constexpr auto ska_sort = ska_sort_t();

struct unique_ska_sort_t {
	static constexpr void operator()(range auto & to_sort, auto const & extract_key) {
		ska_sort(to_sort, extract_key);
		auto const equal = [&](auto const & lhs, auto const & rhs) {
			return extract_key(lhs) == extract_key(rhs);
		};
		::containers::erase_to_end(
			to_sort,
			::containers::unique(containers::begin(to_sort), containers::end(to_sort), equal)
		);
	}
	static constexpr void operator()(range auto & to_sort) {
		operator()(to_sort, to_radix_sort_key);
	}
};
export constexpr auto unique_ska_sort = unique_ska_sort_t();

} // namespace containers
