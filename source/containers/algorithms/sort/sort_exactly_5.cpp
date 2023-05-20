// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.sort_exactly_5;

import containers.algorithms.sort.cheaply_sortable;
import containers.algorithms.sort.dereference_all;
import containers.algorithms.sort.relocate_in_order;
import containers.algorithms.sort.rotate_one;
import containers.algorithms.sort.sort_exactly_4;

import bounded;
import std_module;

namespace containers {
using namespace bounded::literal;

// stable, 5-9 compares (average 7.47), 0-18 relocates (average 10.08)
export constexpr auto sort_exactly_n_impl(auto & x0, auto & x1, auto & x2, auto & x3, auto & x4, auto const compare) -> void {
	using T = std::remove_reference_t<decltype(x0)>;
	::containers::sort_exactly_n_impl(x0, x1, x2, x3, compare);
	if constexpr (cheaply_sortable<T>) {
		if (compare(x4, x3)) {
			std::ranges::swap(x4, x3);
			if (compare(x3, x2)) {
				std::ranges::swap(x3, x2);
				if (compare(x2, x1)) {
					std::ranges::swap(x2, x1);
					if (compare(x1, x0)) {
						std::ranges::swap(x1, x0);
					}
				}
			}
		}
	} else {
		if (compare(x4, x3)) {
			if (compare(x4, x1)) {
				if (compare(x4, x0)) {
					::containers::rotate_one(x4, x3, x2, x1, x0);
				} else {
					::containers::rotate_one(x4, x3, x2, x1);
				}
			} else {
				if (compare(x4, x2)) {
					::containers::rotate_one(x4, x3, x2);
				} else {
					std::ranges::swap(x4, x3);
				}
			}
		} else {
		}
	}
}

export constexpr auto sort_exactly_n(auto it, bounded::constant_t<5> const size, auto const compare) -> void {
	auto [x0, x1, x2, x3, x4, last] = ::containers::dereference_all(std::move(it), size);
	sort_exactly_n_impl(x0, x1, x2, x3, x4, compare);
}

} // namespace containers
