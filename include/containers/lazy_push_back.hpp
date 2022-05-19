// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/uninitialized.hpp>
#include <containers/can_set_size.hpp>
#include <containers/data.hpp>
#include <containers/front_back.hpp>
#include <containers/range_value_t.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>
#include <bounded/lazy_init.hpp>

#include <utility>

namespace containers {
namespace detail {

template<typename Container>
concept member_lazy_push_backable =
	requires(Container container, bounded::function_ptr<range_value_t<Container>> constructor) {
		container.lazy_push_back(constructor);
	};

template<typename Container>
concept lazy_push_backable = member_lazy_push_backable<Container> or can_set_size<Container>;

} // namespace detail

template<detail::lazy_push_backable Container>
constexpr auto & lazy_push_back(
	Container & container,
	bounded::construct_function_for<range_value_t<Container>> auto && constructor
) {
	if constexpr (detail::member_lazy_push_backable<Container>) {
		return container.lazy_push_back(OPERATORS_FORWARD(constructor));
	} else {
		auto const initial_size = containers::size(container);
		auto construct = [&](Container & target) {
			bounded::construct(*(containers::data(target) + initial_size), OPERATORS_FORWARD(constructor));
		};
		if (initial_size < container.capacity()) {
			construct(container);
			container.set_size(initial_size + 1_bi);
		} else if constexpr (detail::reservable<Container>) {
			auto temp = Container();
			temp.reserve(::containers::detail::reallocation_size(container.capacity(), initial_size, 1_bi));
			construct(temp);

			containers::uninitialized_relocate_no_overlap(container, containers::begin(temp));
			container.set_size(0_bi);

			temp.set_size(initial_size + 1_bi);
			container = std::move(temp);
		} else {
			std::unreachable();
		}
		return containers::back(container);
	}
}

} // namespace containers
