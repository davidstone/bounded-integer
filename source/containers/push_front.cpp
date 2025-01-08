// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.push_front;

import containers.front;
import containers.lazy_push_front;
import containers.range_value_t;

import bounded;
import std_module;

namespace containers {

template<typename Container>
concept member_push_frontable = requires(Container & container, range_value_t<Container> value) {
	container.push_front(std::move(value));
};

template<typename Container>
concept push_frontable = member_push_frontable<Container> or lazy_push_frontable<Container>;

export template<push_frontable Container> requires std::copy_constructible<range_value_t<Container>>
constexpr auto push_front(Container & container, range_value_t<Container> const & value) -> auto & {
	if constexpr (member_push_frontable<Container>) {
		container.push_front(value);
		return containers::front(container);
	} else {
		return ::containers::lazy_push_front(
			container,
			::bounded::value_to_function(value)
		);
	}
}
export template<push_frontable Container> requires std::move_constructible<range_value_t<Container>>
constexpr auto push_front(Container & container, range_value_t<Container> && value) -> auto & {
	if constexpr (member_push_frontable<Container>) {
		container.push_front(std::move(value));
		return containers::front(container);
	} else {
		return ::containers::lazy_push_front(
			container,
			::bounded::value_to_function(std::move(value))
		);
	}
}

} // namespace containers
