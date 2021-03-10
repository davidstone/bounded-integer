// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/binary_search.hpp>
#include <containers/associative_container.hpp>

#include <operators/forward.hpp>

namespace containers {

constexpr auto keyed_lower_bound(auto && container, auto && key) {
	return containers::lower_bound(
		OPERATORS_FORWARD(container),
		OPERATORS_FORWARD(key),
		container.compare()
	);
}

constexpr auto keyed_upper_bound(auto && container, auto && key) {
	return containers::upper_bound(
		OPERATORS_FORWARD(container),
		OPERATORS_FORWARD(key),
		container.compare()
	);
}

constexpr auto keyed_equal_range(auto && container, auto && key) {
	return containers::equal_range(
		OPERATORS_FORWARD(container),
		OPERATORS_FORWARD(key),
		container.compare()
	);
}

} // namespace containers
