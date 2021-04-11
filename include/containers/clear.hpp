// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/destroy_range.hpp>
#include <containers/is_empty.hpp>
#include <containers/pop_back.hpp>
#include <containers/pop_front.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {

template<typename Container>
concept member_clearable = requires(Container & container) { container.clear(); };

template<typename Container>
concept clearable = member_clearable<Container> or pop_backable<Container> or pop_frontable<Container>;

} // namespace detail

template<detail::clearable Container>
constexpr auto clear(Container & container) {
	if constexpr (detail::member_clearable<Container>) {
		container.clear();
	} else if constexpr (detail::has_member_append_from_capacity<Container>) {
		// TODO: Link to Godbolt showing better code gen
		containers::destroy_range(container);
		container.append_from_capacity(-containers::size(container));
	} else {
		while (!containers::is_empty(container)) {
			if constexpr (detail::pop_backable<Container>) {
				containers::pop_back(container);
			} else {
				containers::pop_front(container);
			}
		}
	}
}

} // namespace containers
