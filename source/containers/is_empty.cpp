// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.is_empty;

import containers.begin_end;
import containers.range;
import containers.size;
import containers.sized_range;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

export constexpr auto is_empty = []<range Range>(Range && r) {
	// Checking the size is not necessary, but it sometimes leads to slightly
	// more efficient code and it allows more operations to be `constexpr`.
	if constexpr (sized_range<Range>) {
		return containers::size(OPERATORS_FORWARD(r)) == 0_bi;
	} else {
		return
			containers::begin(OPERATORS_FORWARD(r)) ==
			containers::end(OPERATORS_FORWARD(r));
	}
};

} // namespace containers
