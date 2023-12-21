// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.push_back;

import containers.back;
import containers.lazy_push_back;
import containers.member_push_backable;
import containers.range_value_t;

import bounded;
import std_module;

namespace containers {

export template<typename Container>
concept push_backable = member_push_backable<Container> or lazy_push_backable<Container>;

export template<push_backable Container> requires std::copy_constructible<range_value_t<Container>>
constexpr decltype(auto) push_back(Container & container, range_value_t<Container> const & value) {
	if constexpr (member_push_backable<Container>) {
		container.push_back(value);
		return containers::back(container);
	} else {
		return ::containers::lazy_push_back(
			container,
			::bounded::value_to_function(value)
		);
	}
}
export template<push_backable Container> requires std::move_constructible<range_value_t<Container>>
constexpr decltype(auto) push_back(Container & container, range_value_t<Container> && value) {
	if constexpr (member_push_backable<Container>) {
		container.push_back(std::move(value));
		return containers::back(container);
	} else {
		return ::containers::lazy_push_back(
			container,
			::bounded::value_to_function(std::move(value))
		);
	}
}

} // namespace containers