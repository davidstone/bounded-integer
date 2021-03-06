// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/front_back.hpp>
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
concept pop_backable = has_member_pop_back<Container> or has_member_append_from_capacity<Container>;

} // namespace detail

constexpr auto pop_back(detail::pop_backable auto & c) -> void {
	if constexpr (detail::has_member_pop_back<decltype(c)>) {
		c.pop_back();
	} else {
		bounded::destroy(containers::back(c));
		c.append_from_capacity(bounded::constant<-1>);
	}
}

}	// namespace containers
