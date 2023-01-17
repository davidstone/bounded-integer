// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.lazy_push_back;

import containers.algorithms.uninitialized;
import containers.begin_end;
import containers.can_set_size;
import containers.data;
import containers.front_back;
import containers.range_value_t;
import containers.reserve_if_reservable;
import containers.size;

import bounded;
import bounded.test_int;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename Container>
concept member_lazy_push_backable =
	requires(Container container, bounded::function_ptr<range_value_t<Container>> constructor) {
		container.lazy_push_back(constructor);
	};

export template<typename Container>
concept lazy_push_backable = member_lazy_push_backable<Container> or can_set_size<Container>;

export template<lazy_push_backable Container>
constexpr auto & lazy_push_back(
	Container & container,
	bounded::construct_function_for<range_value_t<Container>> auto && constructor
) {
	if constexpr (member_lazy_push_backable<Container>) {
		return container.lazy_push_back(OPERATORS_FORWARD(constructor));
	} else {
		auto const initial_size = containers::size(container);
		auto construct = [&](Container & target) {
			bounded::construct_at(*(containers::data(target) + initial_size), OPERATORS_FORWARD(constructor));
		};
		if (initial_size < container.capacity()) {
			construct(container);
			container.set_size(initial_size + 1_bi);
		} else if constexpr (reservable<Container>) {
			auto temp = Container();
			temp.reserve(::containers::reallocation_size(container.capacity(), initial_size, 1_bi));
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
