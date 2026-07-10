// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.starts_with;

import containers.algorithms.compare;
import containers.range;
import containers.size;
import containers.sized_range;
import containers.take;

namespace containers {

// TODO: Extend to the prefix being an input range
export constexpr auto starts_with(range auto && full, sized_range auto && prefix) -> bool {
	return containers::equal(
		containers::take(OPERATORS_FORWARD(full), containers::size(prefix)),
		OPERATORS_FORWARD(prefix)
	);
}

} // namespace containers
