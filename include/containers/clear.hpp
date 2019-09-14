// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_container.hpp>
#include <containers/pop_back.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {

template<typename Container>
concept member_pop_frontable = requires(Container & container) { container.pop_front(); };

namespace common {

// TODO: Should probably get rid of the fallback to default construction
template<container Container>
constexpr auto clear(Container & container) noexcept {
	if constexpr (pop_backable<Container>) {
		while (!empty(container)) {
			pop_back(container);
		}
	} else if constexpr (member_pop_frontable<Container>) {
		while (!empty(container)) {
			container.pop_front();
		}
	} else {
		container = {};
	}
}

}	// namespace common

using ::containers::detail::common::clear;

}	// namespace detail

using ::containers::detail::common::clear;

}	// namespace containers
