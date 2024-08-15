// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.keyed_binary_search;

import containers.algorithms.binary_search;
import containers.associative_container;

namespace containers {

export constexpr auto keyed_lower_bound(associative_range auto && map, auto && key) {
	return containers::lower_bound(
		OPERATORS_FORWARD(map),
		OPERATORS_FORWARD(key),
		map.compare()
	);
}

export constexpr auto keyed_upper_bound(associative_range auto && map, auto && key) {
	return containers::upper_bound(
		OPERATORS_FORWARD(map),
		OPERATORS_FORWARD(key),
		map.compare()
	);
}

export constexpr auto keyed_equal_range(associative_range auto && map, auto && key) {
	return containers::equal_range(
		OPERATORS_FORWARD(map),
		OPERATORS_FORWARD(key),
		map.compare()
	);
}

} // namespace containers
