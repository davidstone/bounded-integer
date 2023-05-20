// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.sort_exactly_3;

import containers.algorithms.sort.dereference_all;
import containers.algorithms.sort.relocate_in_order;
import containers.algorithms.sort.rotate_one;

import bounded;
import std_module;

namespace containers {

// stable, 2-3 compares (average 2.67), 0-4 relocates (average 2.83)
export constexpr auto sort_exactly_n_impl(auto & x0, auto & x1, auto & x2, auto const compare) -> void {
	if (compare(x1, x0)) {
		if (compare(x2, x1)) {
			std::ranges::swap(x0, x2);
		} else {
			if (compare(x2, x0)) {
				::containers::rotate_one(x0, x1, x2);
			} else {
				std::ranges::swap(x0, x1);
			}
		}
	} else {
		if (compare(x2, x1)) {
			if (compare(x2, x0)) {
				::containers::rotate_one(x0, x2, x1);
			} else {
				std::ranges::swap(x1, x2);
			}
		} else {
		}
	}
}

export constexpr auto sort_exactly_n(auto it, bounded::constant_t<3> const size, auto const compare) -> void {
	auto [x0, x1, x2, last] = ::containers::dereference_all(std::move(it), size);
	sort_exactly_n_impl(x0, x1, x2, compare);
}

export constexpr auto sort_exactly_n_relocate(auto it, bounded::constant_t<3> const size, auto const out, auto const compare) {
	auto [x0, x1, x2, last] = ::containers::dereference_all(std::move(it), size);
	if (compare(x1, x0)) {
		if (compare(x2, x1)) {
			relocate_in_order(out, x2, x1, x0);
		} else {
			if (compare(x2, x0)) {
				relocate_in_order(out, x1, x2, x0);
			} else {
				relocate_in_order(out, x1, x0, x2);
			}
		}
	} else {
		if (compare(x2, x1)) {
			if (compare(x2, x0)) {
				relocate_in_order(out, x2, x0, x1);
			} else {
				relocate_in_order(out, x0, x2, x1);
			}
		} else {
			relocate_in_order(out, x0, x1, x2);
		}
	}
	return last;
}

} // namespace containers
