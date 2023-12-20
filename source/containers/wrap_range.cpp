// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.wrap_range;

import containers.adapt;
import containers.default_adapt_traits;
import containers.default_begin_end_size;
import containers.range;

namespace containers {

struct wrap_traits :
	default_add,
	default_dereference,
	default_subtract,
	default_compare,
	default_begin_end_size
{
};

// Returns an adapted range that behaves the same as the source range. This is
// allows passing a range to an algorithm that wants an adapted range and will
// get at the base iterators.
export constexpr auto wrap_range(range auto && source) {
	return adapt(
		OPERATORS_FORWARD(source),
		wrap_traits()
	);
}

} // namespace containers
