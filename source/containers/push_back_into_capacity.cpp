// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.push_back_into_capacity;

import containers.back;
import containers.is_container;
import containers.lazy_push_back_into_capacity;
import containers.member_push_backable;
import containers.range_value_t;

import bounded;
import std_module;

namespace containers {

export template<typename Container>
concept can_push_back_into_capacity = is_container<Container> and (can_lazy_push_back_into_capacity<Container> or member_push_backable<Container>);

export template<can_push_back_into_capacity Container> requires std::copy_constructible<range_value_t<Container>>
constexpr auto push_back_into_capacity(Container & container, range_value_t<Container> const & value) -> auto & {
	if constexpr (can_lazy_push_back_into_capacity<Container>) {
		return ::containers::lazy_push_back_into_capacity(
			container,
			::bounded::value_to_function(value)
		);
	} else {
		container.push_back(value);
		return containers::back(container);
	}
}
export template<can_push_back_into_capacity Container> requires std::move_constructible<range_value_t<Container>>
constexpr auto push_back_into_capacity(Container & container, range_value_t<Container> && value) -> auto & {
	if constexpr (can_lazy_push_back_into_capacity<Container>) {
		return ::containers::lazy_push_back_into_capacity(
			container,
			::bounded::value_to_function(std::move(value))
		);
	} else {
		container.push_back(std::move(value));
		return containers::back(container);
	}
}

} // namespace containers