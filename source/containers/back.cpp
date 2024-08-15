// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>
#include <bounded/assert.hpp>

export module containers.back;

import containers.algorithms.advance;
import containers.begin_end;
import containers.is_empty;
import containers.range;

namespace containers {

export constexpr auto back(range auto && r) -> decltype(auto) {
	BOUNDED_ASSERT(!containers::is_empty(r));
	return *containers::prev(containers::end(OPERATORS_FORWARD(r)));
}

} // namespace containers