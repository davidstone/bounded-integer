// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.copy_or_relocate_from;

import containers.begin_end;
import containers.can_set_size;
import containers.dereference;
import containers.is_container;
import containers.iterator_t;
import containers.range;
import containers.range_value_t;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

export constexpr auto copy_or_relocate_from = []<range Input>(Input && input, auto function) -> iterator_t<Input> {
	// We do not relocate trivially copyable types because that requires setting
	// the size of the source range to 0. This is extra work that is not
	// necessary -- we don't want to make operations on rvalues more expensive
	// than operations on lvalues.
	//
	// TODO: Optimize node-based input containers
	constexpr auto use_relocate =
		is_container<Input> and
		can_set_size<Input> and
		!std::is_trivially_copyable_v<range_value_t<Input>>;
	auto it = containers::begin(OPERATORS_FORWARD(input));
	auto const last = containers::end(OPERATORS_FORWARD(input));
	if constexpr (use_relocate) {
		for (; it != last; ++it) {
			function([&] { return bounded::relocate(*it); });
		}
		input.set_size(0_bi);
	} else {
		for (; it != last; ++it) {
			function([&] -> decltype(auto) { return dereference<Input>(it); });
		}
	}
	return it;
};

} // namespace containers
