// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.sort_exactly_3;

import containers.algorithms.sort.cheaply_sortable;
import containers.algorithms.sort.low_high_ref;
import containers.algorithms.sort.relocate_in_order;
import containers.algorithms.sort.rotate_one;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

// Stable
// Average compares: 2.67 (weight 16)
// Max compares: 3
// Sorted compares: 2
// Reversed compares: 2
export template<typename T>
constexpr auto sort_exactly_n_in_place_impl(
	T & x0,
	T & x1,
	T & x2,
	auto const compare
) -> void {
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

export template<typename T, typename Compare>
constexpr auto sort_exactly_n_relocate_impl(
	T & x0,
	T & x1,
	T & x2,
	auto const out,
	Compare const compare
) -> void {
	if constexpr (cheaply_sortable<T, Compare>) {
		auto const first_half_in_order = !compare(x1, x0);
		auto const first_half = first_half_in_order ?
			low_high_ref(x0, x1) :
			low_high_ref(x1, x0);
		auto const lowest_in_first = !compare(x2, first_half.low);
		auto const highest_in_first = compare(x2, first_half.high);
		using ref_t = std::reference_wrapper<std::remove_reference_t<decltype(x0)>>;
		auto const lowest = lowest_in_first ? ref_t(first_half.low) : ref_t(x2);
		auto const middle =
			!lowest_in_first ? ref_t(first_half.low) :
			!highest_in_first ? ref_t(first_half.high) :
			ref_t(x2);
		auto const highest = highest_in_first ? ref_t(first_half.high) : ref_t(x2);
		::containers::relocate_in_order(out, lowest.get(), middle.get(), highest.get());
	} else {
		if (compare(x1, x0)) {
			if (compare(x2, x1)) {
				::containers::relocate_in_order(out, x2, x1, x0);
			} else {
				if (compare(x2, x0)) {
					::containers::relocate_in_order(out, x1, x2, x0);
				} else {
					::containers::relocate_in_order(out, x1, x0, x2);
				}
			}
		} else {
			if (compare(x2, x1)) {
				if (compare(x2, x0)) {
					::containers::relocate_in_order(out, x2, x0, x1);
				} else {
					::containers::relocate_in_order(out, x0, x2, x1);
				}
			} else {
				::containers::relocate_in_order(out, x0, x1, x2);
			}
		}
	}
}

} // namespace containers
