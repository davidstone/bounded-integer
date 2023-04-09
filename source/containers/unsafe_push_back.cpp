// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.unsafe_push_back;

import containers.front_back;
import containers.is_container;
import containers.member_push_backable;
import containers.range_value_t;
import containers.unsafe_lazy_push_back;

import bounded;
import std_module;

namespace containers {

export template<typename Container>
concept unsafe_push_backable = is_container<Container> and (unsafe_lazy_push_backable<Container> or member_push_backable<Container>);

export template<unsafe_push_backable Container> requires std::copy_constructible<range_value_t<Container>>
constexpr auto unsafe_push_back(Container & container, range_value_t<Container> const & value) -> auto & {
	if constexpr (unsafe_lazy_push_backable<Container>) {
		return ::containers::unsafe_lazy_push_back(
			container,
			::bounded::value_to_function(value)
		);
	} else {
		container.push_back(value);
		return containers::back(container);
	}
}
export template<unsafe_push_backable Container> requires std::move_constructible<range_value_t<Container>>
constexpr auto unsafe_push_back(Container & container, range_value_t<Container> && value) -> auto & {
	if constexpr (unsafe_lazy_push_backable<Container>) {
		return ::containers::unsafe_lazy_push_back(
			container,
			::bounded::value_to_function(std::move(value))
		);
	} else {
		container.push_back(std::move(value));
		return containers::back(container);
	}
}

} // namespace containers