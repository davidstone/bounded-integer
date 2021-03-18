// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/associative_container.hpp>

#include <utility>

namespace containers {
namespace detail {

template<typename Map>
concept legacy_map = requires { typename Map::allocator_type; };

} // namespace detail

template<associative_container Container>
constexpr auto keyed_insert(Container & container, typename Container::value_type const & value) {
	if constexpr (detail::legacy_map<Container>) {
		return container.insert(value);
	} else {
		return container.lazy_insert(value.key(), [&] { return value.mapped(); });
	}
}
template<associative_container Container>
constexpr auto keyed_insert(Container & container, typename Container::value_type && value) {
	if constexpr (detail::legacy_map<Container>) {
		return container.insert(std::move(value));
	} else {
		return container.lazy_insert(std::move(value).key(), [&] { return std::move(value).mapped(); });
	}
}

} // namespace containers
