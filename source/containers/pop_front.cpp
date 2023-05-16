// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.pop_front;

import containers.algorithms.erase;
import containers.begin_end;
import containers.erase_concepts;

namespace containers {

export template<pop_frontable Container>
constexpr auto pop_front(Container & c) -> void {
	if constexpr (member_pop_frontable<Container>) {
		c.pop_front();
	} else if constexpr (constant_time_erasable<Container>) {
		containers::erase(c, containers::begin(c));
	} else {
		c.erase_after(c.before_begin());
	}
}

} // namespace containers