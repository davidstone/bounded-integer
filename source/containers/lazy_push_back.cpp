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
import containers.front_back;
import containers.member_lazy_push_backable;
import containers.range_value_t;
import containers.reallocation_size;
import containers.reserve_if_reservable;
import containers.size;
import containers.unsafe_lazy_push_back;

import bounded;
import bounded.test_int;
import std_module;

using namespace bounded::literal;

namespace containers {

export template<typename Container>
concept lazy_push_backable = member_lazy_push_backable<Container> or can_set_size<Container>;

// Requires that the type has a member function `lazy_push_back` or that
// `*containers::end(container)` produces a reference to writable memory
export template<lazy_push_backable Container>
constexpr auto & lazy_push_back(
	Container & container,
	bounded::construct_function_for<range_value_t<Container>> auto && constructor
) {
	if constexpr (member_lazy_push_backable<Container>) {
		return container.lazy_push_back(OPERATORS_FORWARD(constructor));
	} else {
		auto const initial_size = containers::size(container);
		if (initial_size < container.capacity()) {
			return ::containers::unsafe_lazy_push_back(container, OPERATORS_FORWARD(constructor));
		} else if constexpr (reservable<Container>) {
			auto temp = Container();
			temp.reserve(::containers::reallocation_size(container.capacity(), initial_size, 1_bi));

			bounded::construct_at(*(containers::begin(temp) + initial_size), OPERATORS_FORWARD(constructor));

			containers::uninitialized_relocate_no_overlap(container, containers::begin(temp));
			container.set_size(0_bi);

			temp.set_size(initial_size + 1_bi);

			container = std::move(temp);
		} else {
			throw std::bad_alloc();
		}
		return containers::back(container);
	}
}

} // namespace containers
