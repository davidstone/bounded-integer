// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/uninitialized.hpp>
#include <containers/data.hpp>
#include <containers/front_back.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

namespace containers {
namespace detail {

template<typename Container, typename... Args>
concept member_emplace_backable = requires(Container & container, Args && ... args) {
	container.emplace_back(BOUNDED_FORWARD(args)...);
};

} // namespace detail

template<typename Container, typename... Args>
constexpr auto & emplace_back(Container & container, Args && ... args) {
	if constexpr (detail::member_emplace_backable<Container, Args...>) {
		return container.emplace_back(BOUNDED_FORWARD(args)...);
	} else {
		auto const initial_size = size(container);
		if (initial_size < container.capacity()) {
			bounded::construct(*(data(container) + initial_size), BOUNDED_FORWARD(args)...);
			container.append_from_capacity(1_bi);
		} else if constexpr (detail::has_reserve<Container>) {
			auto temp = Container();
			temp.reserve(::containers::detail::reallocation_size(container, 1_bi));
			auto & ref = *(data(temp) + container.capacity());
			bounded::construct(ref, BOUNDED_FORWARD(args)...);
			try {
				containers::detail::transfer_all_contents(container, temp);
			} catch (...) {
				bounded::destroy(ref);
				throw;
			}
			temp.append_from_capacity(initial_size + 1_bi);
			container = std::move(temp);
		} else {
			BOUNDED_ASSERT_OR_ASSUME(false);
		}
		return back(container);
	}
}
	
}	// namespace containers
