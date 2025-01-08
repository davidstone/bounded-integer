// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.lazy_push_front;

import containers.algorithms.splice;
import containers.begin_end;
import containers.front;
import containers.lazy_push_back;
import containers.range_value_t;
import containers.splicable;
import containers.supports_lazy_insert_after;

import bounded;

namespace containers {

export template<typename Container>
concept lazy_push_frontable =
	supports_lazy_insert_after<Container> or
	(bounded::default_constructible<Container> and lazy_push_backable<Container> and splicable<Container>);


export template<lazy_push_frontable Container>
constexpr auto lazy_push_front(
	Container & container,
	bounded::construct_function_for<range_value_t<Container>> auto && constructor
) -> auto & {
	if constexpr (supports_lazy_insert_after<Container>) {
		return *container.lazy_insert_after(container.before_begin(), OPERATORS_FORWARD(constructor));
	} else {
		auto temp = Container();
		::containers::lazy_push_back(temp, OPERATORS_FORWARD(constructor));
		::containers::splice(container, containers::begin(container), temp);
		return containers::front(container);
	}
}

} // namespace containers
