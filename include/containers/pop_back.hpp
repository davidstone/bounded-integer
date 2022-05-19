// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/algorithms/erase.hpp>
#include <containers/begin_end.hpp>
#include <containers/can_set_size.hpp>
#include <containers/erase_concepts.hpp>
#include <containers/front_back.hpp>

#include <bounded/detail/construct_destroy.hpp>
#include <bounded/integer.hpp>

namespace containers {

template<detail::pop_backable Container>
constexpr auto pop_back(Container & c) -> void {
	if constexpr (detail::member_pop_backable<Container>) {
		c.pop_back();
	} else if constexpr (detail::can_set_size<Container>) {
		bounded::destroy(containers::back(c));
		c.set_size(containers::size(c) - 1_bi);
	} else {
		containers::erase(c, containers::prev(containers::end(c)));
	}
}

} // namespace containers
