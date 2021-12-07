// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/associative_container.hpp>
#include <containers/range_value_t.hpp>

#include <utility>

namespace containers {
namespace detail {

template<typename Map>
concept legacy_map = requires { typename Map::allocator_type; };

} // namespace detail

template<
	associative_container Container,
	std::convertible_to<typename Container::key_type> Key = typename Container::key_type,
	std::convertible_to<typename Container::mapped_type> Mapped = typename Container::mapped_type
>
constexpr auto keyed_insert(Container & container, Key && key, Mapped && mapped) {
	if constexpr (detail::legacy_map<Container>) {
		return container.insert({OPERATORS_FORWARD(key), OPERATORS_FORWARD(mapped)});
	} else {
		return container.lazy_insert(OPERATORS_FORWARD(key), [&] { return OPERATORS_FORWARD(mapped); });
	}
}
template<associative_container Container>
constexpr auto keyed_insert(Container & container, range_value_t<Container> const & value) {
	if constexpr (detail::legacy_map<Container>) {
		return container.insert(value);
	} else {
		return container.lazy_insert(get_key(value), [&] { return get_mapped(value); });
	}
}
template<associative_container Container>
constexpr auto keyed_insert(Container & container, range_value_t<Container> && value) {
	if constexpr (detail::legacy_map<Container>) {
		return container.insert(std::move(value));
	} else {
		return container.lazy_insert(get_key(std::move(value)), [&] { return get_mapped(std::move(value)); });
	}
}

} // namespace containers
