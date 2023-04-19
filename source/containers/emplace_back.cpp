// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.emplace_back;

import containers.lazy_push_back;
import containers.range_value_t;

import bounded;

namespace containers {

template<typename Container, typename... Args>
concept member_emplace_backable = requires(Container & container, Args && ... args) {
	container.emplace_back(OPERATORS_FORWARD(args)...);
};

export template<typename Container, typename... Args>
constexpr auto & emplace_back(Container & container, Args && ... args) {
	if constexpr (member_emplace_backable<Container, Args...>) {
		return container.emplace_back(OPERATORS_FORWARD(args)...);
	} else {
		return ::containers::lazy_push_back(container, [&]{
			return bounded::construct<range_value_t<Container>>(OPERATORS_FORWARD(args)...);
		});
	}
}

} // namespace containers