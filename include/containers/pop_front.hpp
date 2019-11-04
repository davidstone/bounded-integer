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
concept pop_frontable = is_container<Container> and requires (Container & container) { container.pop_front(); };

} // namespace detail

constexpr auto pop_front(detail::pop_frontable auto & c) -> void {
	c.pop_front();
}

}	// namespace containers
