// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/erase.hpp>
#include <containers/begin_end.hpp>
#include <containers/front_back.hpp>
#include <containers/constant_time_erasable.hpp>
#include <containers/is_range.hpp>

#include <bounded/detail/construct_destroy.hpp>
#include <bounded/integer.hpp>

namespace containers {
namespace detail {

template<typename Container>
concept member_pop_frontable = requires (Container & container) { container.pop_front(); };

template<typename Container>
concept pop_frontable = member_pop_frontable<Container> or constant_time_erasable<Container>;

} // namespace detail

template<detail::pop_frontable Container>
constexpr auto pop_front(Container & c) -> void {
	if constexpr (detail::member_pop_frontable<Container>) {
		c.pop_front();
	} else {
		static_assert(constant_time_erasable<Container>);
		containers::erase(c, containers::begin(c));
	}
}

} // namespace containers
