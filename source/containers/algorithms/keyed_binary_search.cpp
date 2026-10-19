// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.keyed_binary_search;

import containers.algorithms.binary_search;
import containers.algorithms.transform;
import containers.associative_container;
import containers.map_value_type;
import containers.subrange;

import std_module;

namespace containers {

export constexpr auto keyed_lower_bound(map_range auto && map, auto && key) {
	return containers::lower_bound(
		containers::transform(OPERATORS_FORWARD(map), get_key),
		OPERATORS_FORWARD(key),
		map.key_comp()
	).base();
}
export constexpr auto keyed_lower_bound(set_range auto && set, auto && key) {
	return containers::lower_bound(
		OPERATORS_FORWARD(set),
		OPERATORS_FORWARD(key),
		set.key_comp()
	);
}

export constexpr auto keyed_upper_bound(map_range auto && map, auto && key) {
	return containers::upper_bound(
		containers::transform(OPERATORS_FORWARD(map), get_key),
		OPERATORS_FORWARD(key),
		map.key_comp()
	).base();
}
export constexpr auto keyed_upper_bound(set_range auto && set, auto && key) {
	return containers::upper_bound(
		OPERATORS_FORWARD(set),
		OPERATORS_FORWARD(key),
		set.key_comp()
	);
}

export constexpr auto keyed_equal_range(map_range auto && map, auto && key) {
	auto temp = containers::equal_range(
		containers::transform(OPERATORS_FORWARD(map), get_key),
		OPERATORS_FORWARD(key),
		map.key_comp()
	);
	return subrange(
		std::move(temp).begin().base(),
		std::move(temp).end().base()
	);
}
export constexpr auto keyed_equal_range(set_range auto && set, auto && key) {
	return containers::equal_range(
		OPERATORS_FORWARD(set),
		OPERATORS_FORWARD(key),
		set.key_comp()
	);
}

} // namespace containers
