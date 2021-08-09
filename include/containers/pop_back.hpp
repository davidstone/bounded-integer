// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
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
concept has_member_pop_back = requires (Container & container) { container.pop_back(); };

template<typename Container>
concept has_member_append_from_capacity = requires (Container & container) { container.append_from_capacity(bounded::constant<-1>); };

template<typename Container>
concept pop_backable = has_member_pop_back<Container> or has_member_append_from_capacity<Container> or constant_time_erasable<Container>;

} // namespace detail

template<detail::pop_backable Container>
constexpr auto pop_back(Container & c) -> void {
	if constexpr (detail::has_member_pop_back<Container>) {
		c.pop_back();
	} else if constexpr (constant_time_erasable<Container>) {
		containers::erase(c, containers::prev(containers::end(c)));
	} else {
		bounded::destroy(containers::back(c));
		c.append_from_capacity(bounded::constant<-1>);
	}
}

}	// namespace containers
