// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.member_push_backable;

import containers.range_value_t;

import std_module;

namespace containers {

export template<typename Container>
concept member_push_backable = requires(Container & container, range_value_t<Container> value) {
	container.push_back(std::move(value));
};

} // namespace containers