// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.ends_with;

import containers.algorithms.compare;
import containers.algorithms.drop;
import containers.range_size_t;
import containers.size;
import containers.sized_range;

import bounded;
import numeric_traits;

namespace containers {

// TODO: support `Full` and `Suffix` both being bidirectional ranges
export template<sized_range Full, sized_range Suffix>
constexpr auto ends_with(Full && full, Suffix && suffix) -> bool {
	if constexpr (numeric_traits::max_value<range_size_t<Full>> < numeric_traits::min_value<range_size_t<Suffix>>) {
		return false;
	} else {
		auto const full_size = containers::size(full);
		auto const suffix_size = containers::size(suffix);
		if (suffix_size > full_size) {
			return false;
		}
		auto const prefix_size = bounded::increase_min<0>(full_size - suffix_size);
		return containers::equal(
			containers::drop_exactly(OPERATORS_FORWARD(full), prefix_size),
			OPERATORS_FORWARD(suffix)
		);
	}
}

} // namespace containers
