// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.sort_exactly_2;

import containers.algorithms.sort.relocate_in_order;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

// Stable
// Average compares: 1 (weight 1)
// Max compares: 1
// Sorted compares: 1
// Reversed compares: 1
export template<typename T>
constexpr auto sort_exactly_n_in_place_impl(
	T & x0,
	T & x1,
	auto const compare
) -> void {
	if (compare(x1, x0)) {
		std::ranges::swap(x0, x1);
	}
}

export template<typename T>
constexpr auto sort_exactly_n_relocate_impl(
	T & x0,
	T & x1,
	auto const out,
	auto const compare
) -> void {
	if (compare(x1, x0)) {
		::containers::relocate_in_order(out, x1, x0);
	} else {
		::containers::relocate_in_order(out, x0, x1);
	}
}

} // namespace containers
