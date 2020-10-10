// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/pop_back.hpp>
#include <containers/pop_front.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {
namespace common {

template<typename Container> requires pop_backable<Container> or pop_frontable<Container>
constexpr auto clear(Container & value) {
	while (!empty(value)) {
		if constexpr (pop_backable<Container>) {
			pop_back(value);
		} else {
			pop_front(value);
		}
	}
}

}	// namespace common

using ::containers::detail::common::clear;

}	// namespace detail

using ::containers::detail::common::clear;

}	// namespace containers
