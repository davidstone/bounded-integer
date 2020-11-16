// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_empty.hpp>
#include <containers/pop_back.hpp>
#include <containers/pop_front.hpp>

#include <bounded/integer.hpp>

namespace containers {

template<typename Container> requires detail::pop_backable<Container> or detail::pop_frontable<Container>
constexpr auto clear(Container & value) {
	while (!containers::is_empty(value)) {
		if constexpr (detail::pop_backable<Container>) {
			containers::pop_back(value);
		} else {
			containers::pop_front(value);
		}
	}
}

} // namespace containers
