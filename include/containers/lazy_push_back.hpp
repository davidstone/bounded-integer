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
#include <bounded/lazy_init.hpp>
#include <bounded/unreachable.hpp>

namespace containers {
namespace detail {

template<typename Container>
concept appendable_from_capacity = requires(Container & container, typename Container::size_type count) { container.append_from_capacity(count); };

} // namespace detail

template<detail::appendable_from_capacity Container>
constexpr auto & lazy_push_back(
	Container & container,
	bounded::construct_function_for<typename Container::value_type> auto && constructor
) {
	auto const initial_size = size(container);
	auto construct = [&](auto & ref) {
		bounded::construct(ref, OPERATORS_FORWARD(constructor));
	};
	if (initial_size < container.capacity()) {
		construct(*(data(container) + initial_size));
		container.append_from_capacity(1_bi);
	} else if constexpr (detail::reservable<Container>) {
		auto temp = Container();
		temp.reserve(::containers::detail::reallocation_size(container, 1_bi));
		auto & ref = *(data(temp) + container.capacity());
		construct(ref);
		containers::detail::transfer_all_contents(container, temp);
		temp.append_from_capacity(initial_size + 1_bi);
		container = std::move(temp);
	} else {
		bounded::assert_or_assume_unreachable();
	}
	return back(container);
}
	
}	// namespace containers
