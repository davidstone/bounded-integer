// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.clear;

import containers.algorithms.destroy_range;
import containers.can_set_size;
import containers.erase_concepts;
import containers.is_empty;
import containers.pop_back;
import containers.pop_front;
import containers.size;

import bounded;

using namespace bounded::literal;

namespace containers {

template<typename Container>
concept member_clearable = requires(Container & container) { container.clear(); };

template<typename Container>
concept clearable = member_clearable<Container> or pop_backable<Container> or pop_frontable<Container>;

export template<clearable Container>
constexpr auto clear(Container & container) {
	if constexpr (member_clearable<Container>) {
		container.clear();
	} else if constexpr (can_set_size<Container>) {
		// TODO: Link to Godbolt showing better code gen
		containers::destroy_range(container);
		container.set_size(0_bi);
	} else {
		while (!containers::is_empty(container)) {
			if constexpr (pop_backable<Container>) {
				containers::pop_back(container);
			} else {
				containers::pop_front(container);
			}
		}
	}
}

} // namespace containers