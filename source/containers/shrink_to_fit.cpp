// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.shrink_to_fit;

import containers.algorithms.uninitialized;
import containers.begin_end;
import containers.range;
import containers.size;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace containers {

export template<range Container>
constexpr auto shrink_to_fit(Container & c) {
	auto const s = containers::size(c);
	if (s == c.capacity()) {
		return;
	}
	constexpr auto min_capacity = numeric_traits::min_value<decltype(c.capacity())>;
	if constexpr (min_capacity > 0_bi) {
		if (c.capacity() == min_capacity) {
			return;
		}
	}
	auto temp = Container();
	temp.reserve(s);
	containers::uninitialized_relocate_no_overlap(c, containers::begin(temp));
	temp.set_size(s);
	c.set_size(0_bi);
	c = std::move(temp);
}

} // namespace containers
