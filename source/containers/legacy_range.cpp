// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Adapter for functions that cannot be made compatible with a bounded::integer

module;

#include <operators/forward.hpp>

export module containers.legacy_range;

import containers.begin_end;
import containers.legacy_iterator;
import containers.subrange;

namespace containers {

export constexpr auto legacy_range(auto && r) {
	return containers::subrange(
		maybe_legacy_iterator(containers::begin(OPERATORS_FORWARD(r))),
		maybe_legacy_iterator(containers::end(OPERATORS_FORWARD(r)))
	);
}

} // namespace containers
