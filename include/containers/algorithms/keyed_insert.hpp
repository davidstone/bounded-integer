// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/associative_container.hpp>
#include <containers/map_value_type.hpp>
#include <containers/range_value_t.hpp>

#include <utility>

namespace containers {
namespace detail {

template<typename Map>
concept legacy_map = requires { typename Map::allocator_type; };

} // namespace detail

template<
	associative_container Map,
	bounded::convertible_to<typename Map::key_type> Key = typename Map::key_type,
	bounded::convertible_to<typename Map::mapped_type> Mapped = typename Map::mapped_type
>
constexpr auto keyed_insert(Map & map, Key && key, Mapped && mapped) {
	if constexpr (detail::legacy_map<Map>) {
		return map.insert({OPERATORS_FORWARD(key), OPERATORS_FORWARD(mapped)});
	} else {
		return map.lazy_insert(OPERATORS_FORWARD(key), [&] { return OPERATORS_FORWARD(mapped); });
	}
}
template<associative_container Map>
constexpr auto keyed_insert(Map & map, range_value_t<Map> const & value) {
	if constexpr (detail::legacy_map<Map>) {
		return map.insert(value);
	} else {
		return map.lazy_insert(get_key(value), [&] { return get_mapped(value); });
	}
}
template<associative_container Map>
constexpr auto keyed_insert(Map & map, range_value_t<Map> && value) {
	if constexpr (detail::legacy_map<Map>) {
		return map.insert(std::move(value));
	} else {
		return map.lazy_insert(get_key(std::move(value)), [&] { return get_mapped(std::move(value)); });
	}
}

} // namespace containers
