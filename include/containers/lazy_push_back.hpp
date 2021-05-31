// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/uninitialized.hpp>
#include <containers/data.hpp>
#include <containers/front_back.hpp>
#include <containers/range_value_t.hpp>
#include <containers/reserve_if_reservable.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>
#include <bounded/lazy_init.hpp>
#include <bounded/unreachable.hpp>

namespace containers {
namespace detail {

template<typename Container>
concept appendable_from_capacity = range<Container> and requires(Container & container, range_size_t<Container> count) { container.append_from_capacity(count); };

template<typename Container>
concept lazy_push_backable = appendable_from_capacity<Container>;

} // namespace detail

template<detail::lazy_push_backable Container>
constexpr auto & lazy_push_back(
	Container & container,
	bounded::construct_function_for<range_value_t<Container>> auto && constructor
) {
	auto const initial_size = containers::size(container);
	auto construct = [&](Container & target) {
		bounded::construct(*(containers::data(target) + initial_size), OPERATORS_FORWARD(constructor));
	};
	if (initial_size < container.capacity()) {
		construct(container);
		container.append_from_capacity(1_bi);
	} else if constexpr (detail::reservable<Container>) {
		auto temp = Container();
		temp.reserve(::containers::detail::reallocation_size(container.capacity(), initial_size, 1_bi));
		construct(temp);

		containers::uninitialized_relocate_no_overlap(container, containers::begin(temp));
		container.append_from_capacity(-initial_size);

		temp.append_from_capacity(initial_size + 1_bi);
		container = std::move(temp);
	} else {
		bounded::assert_or_assume_unreachable();
	}
	return containers::back(container);
}

} // namespace containers
