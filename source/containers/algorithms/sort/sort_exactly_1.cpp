// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.sort_exactly_1;

import containers.iterator;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

// Stable
// Average compares: 0 (weight 0)
// Max compares: 0
// Sorted compares: 0
// Reversed compares: 0
export template<iterator Iterator, typename Compare>
constexpr auto sort_exactly_n_in_place(Iterator, bounded::constant_t<1>, Compare) -> void {
}

export template<typename T, typename Compare>
constexpr auto sort_exactly_n_relocate_impl(
	T & x0,
	auto const out,
	Compare
) -> void {
	bounded::relocate_at(*out, x0);
}

} // namespace containers
