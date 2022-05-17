// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/front_back.hpp>
#include <containers/iterator_t.hpp>
#include <containers/lazy_push_back.hpp>
#include <containers/range_value_t.hpp>
#include <containers/splicable.hpp>
#include <containers/supports_lazy_insert_after.hpp>

#include <bounded/concepts.hpp>
#include <bounded/lazy_init.hpp>

#include <operators/forward.hpp>

#include <concepts>
#include <type_traits>

namespace containers {
namespace detail {

template<typename Container>
concept lazy_push_frontable =
	supports_lazy_insert_after<Container> or
	(std::is_default_constructible_v<Container> and lazy_push_backable<Container> and splicable<Container>);

} // namespace detail

template<detail::lazy_push_frontable Container>
constexpr auto lazy_push_front(
	Container & container,
	bounded::construct_function_for<range_value_t<Container>> auto && constructor
) -> auto & {
	if constexpr (detail::supports_lazy_insert_after<Container>) {
		return *container.lazy_insert_after(container.before_begin(), OPERATORS_FORWARD(constructor));
	} else {
		auto temp = Container();
		containers::lazy_push_back(temp, OPERATORS_FORWARD(constructor));
		container.splice(containers::begin(container), temp);
		return containers::front(container);
	}
}

} // namespace containers
