// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.emplace_front;

import containers.lazy_push_front;
import containers.range_value_t;

namespace containers {

template<typename Container, typename... Args>
concept member_emplace_frontable = requires(Container & container, Args && ... args) {
	container.emplace_front(OPERATORS_FORWARD(args)...);
};

export template<typename Container, typename... Args>
constexpr auto & emplace_front(Container & container, Args && ... args) {
	if constexpr (member_emplace_frontable<Container, Args...>) {
		return container.emplace_front(OPERATORS_FORWARD(args)...);
	} else {
		return ::containers::lazy_push_front(container, [&]{
			return range_value_t<Container>(OPERATORS_FORWARD(args)...);
		});
	}
}

} // namespace containers