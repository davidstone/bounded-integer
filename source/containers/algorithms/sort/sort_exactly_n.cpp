// Copyright David Stone 2025.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.sort_exactly_n;

import containers.algorithms.sort.cheaply_sortable;
import containers.algorithms.sort.dereference_all;
export import containers.algorithms.sort.sort_exactly_1;
export import containers.algorithms.sort.sort_exactly_2;
export import containers.algorithms.sort.sort_exactly_3;
export import containers.algorithms.sort.sort_exactly_4;
export import containers.algorithms.sort.sort_exactly_5;
export import containers.algorithms.sort.sort_exactly_6;
import containers.algorithms.uninitialized;
import containers.iter_value_t;
import containers.iterator;
import containers.uninitialized_array;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

export template<iterator Iterator, typename Compare>
constexpr auto sort_exactly_n_in_place(Iterator it, auto const size, Compare const compare) -> void {
	auto dereferenced = ::containers::dereference_all(it, size);
	auto [...elements] = dereferenced.elements;
	::containers::sort_exactly_n_in_place_impl(elements.get()..., compare);
}

export template<iterator Iterator>
constexpr auto sort_exactly_n_relocate(Iterator it, auto const size, auto const out, auto const compare) -> Iterator {
	auto dereferenced = ::containers::dereference_all(it, size);
	auto [...elements] = dereferenced.elements;
	::containers::sort_exactly_n_relocate_impl(elements.get()..., out, compare);
	return dereferenced.last;
}

export template<iterator Iterator, typename Compare>
constexpr auto sort_exactly_n(Iterator it, auto const size, Compare const compare) -> void {
	using T = iter_value_t<Iterator>;
	if constexpr (cheaply_sortable<T, Compare> and size <= 4_bi) {
		auto temp = containers::uninitialized_array<T, size>();
		sort_exactly_n_relocate(it, size, temp.data(), compare);
		containers::uninitialized_relocate_no_overlap(temp, it);
	} else {
		::containers::sort_exactly_n_in_place(it, size, compare);
	}
}

} // namespace containers
