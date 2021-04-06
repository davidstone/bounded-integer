// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/uninitialized.hpp>
#include <containers/front_back.hpp>
#include <containers/lazy_push_back.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/range_value_t.hpp>

#include <bounded/integer.hpp>
#include <bounded/unreachable.hpp>

namespace containers {
namespace detail {

template<typename Container, typename... Args>
concept member_emplace_backable = requires(Container & container, Args && ... args) {
	container.emplace_back(OPERATORS_FORWARD(args)...);
};

} // namespace detail

template<typename Container, typename... Args>
constexpr auto & emplace_back(Container & container, Args && ... args) {
	if constexpr (detail::member_emplace_backable<Container, Args...>) {
		return container.emplace_back(OPERATORS_FORWARD(args)...);
	} else {
		return ::containers::lazy_push_back(container, [&]{
			return bounded::construct_return<range_value_t<Container>>(OPERATORS_FORWARD(args)...);
		});
	}
}
	
}	// namespace containers
