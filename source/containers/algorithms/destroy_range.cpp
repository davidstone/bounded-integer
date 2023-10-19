// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.destroy_range;

import containers.range;

import bounded;

namespace containers {

export constexpr void destroy_range(range auto && r) {
	for (auto & value : OPERATORS_FORWARD(r)) {
		bounded::destroy(value);
	}
}

} // namespace containers
