// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.emplace_back_into_capacity;

import containers.is_container;
import containers.lazy_push_back_into_capacity;
import containers.range_value_t;

import bounded;

namespace containers {

template<typename Container, typename... Args>
concept member_emplace_backable = requires(Container & container, Args && ... args) {
	container.emplace_back(OPERATORS_FORWARD(args)...);
};

export template<typename Container>
concept can_emplace_back_into_capacity = is_container<Container> and (can_lazy_push_back_into_capacity<Container> or member_emplace_backable<Container>);

export template<can_emplace_back_into_capacity Container, typename... Args>
	requires bounded::constructible_from<range_value_t<Container>, Args...>
constexpr auto emplace_back_into_capacity(Container & container, Args && ... args) -> auto & {
	if constexpr (can_lazy_push_back_into_capacity<Container>) {
		return ::containers::lazy_push_back_into_capacity(container, [&]{
			return range_value_t<Container>(OPERATORS_FORWARD(args)...);
		});
	} else {
		return container.emplace_back(OPERATORS_FORWARD(args)...);
	}
}

} // namespace containers