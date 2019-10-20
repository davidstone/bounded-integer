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
constexpr auto clear(container auto & value) {
	using Container = decltype(value);
	if constexpr (pop_backable<Container>) {
		while (!empty(value)) {
			pop_back(value);
		}
	} else if constexpr (member_pop_frontable<Container>) {
		while (!empty(value)) {
			value.pop_front();
		}
	} else {
		value = {};
	}
}

}	// namespace common

using ::containers::detail::common::clear;

}	// namespace detail

using ::containers::detail::common::clear;

}	// namespace containers
