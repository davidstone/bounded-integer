// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.pop_back;

import containers.back;
import containers.can_set_size;
import containers.range;
import containers.size;

import bounded;

using namespace bounded::literal;

namespace containers {

template<typename Container>
concept member_pop_backable = requires (Container & container) { container.pop_back(); };

export template<typename Container>
concept pop_backable =
	range<Container> and (
		member_pop_backable<Container> or
		can_set_size<Container>
	);

export template<pop_backable Container>
constexpr auto pop_back(Container & c) -> void {
	if constexpr (member_pop_backable<Container>) {
		c.pop_back();
	} else {
		bounded::destroy(containers::back(c));
		c.set_size(containers::size(c) - 1_bi);
	}
}

} // namespace containers
