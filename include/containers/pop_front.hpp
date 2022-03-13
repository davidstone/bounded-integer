// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/erase.hpp>
#include <containers/begin_end.hpp>
#include <containers/erase_concepts.hpp>

namespace containers {

template<detail::pop_frontable Container>
constexpr auto pop_front(Container & c) -> void {
	if constexpr (detail::member_pop_frontable<Container>) {
		c.pop_front();
	} else if constexpr (detail::constant_time_erasable<Container>) {
		containers::erase(c, containers::begin(c));
	} else {
		c.erase_after(c.before_begin());
	}
}

} // namespace containers
