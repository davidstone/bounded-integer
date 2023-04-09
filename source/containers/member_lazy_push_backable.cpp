// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.member_lazy_push_backable;

import containers.range_value_t;

import bounded;

namespace containers {

export template<typename Container>
concept member_lazy_push_backable =
	requires(Container container, bounded::function_ptr<range_value_t<Container>> constructor) {
		container.lazy_push_back(constructor);
	};

} // namespace containers
