// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.join;

import containers.algorithms.join_with;
import containers.array;
import containers.range;
import containers.range_value_t;

import bounded;

namespace containers {
using namespace bounded::literal;

export template<range Range>
constexpr auto join(Range && r) {
	using value_type = containers::range_value_t<containers::range_value_t<Range>>;
	return ::containers::join_with(
		OPERATORS_FORWARD(r),
		containers::array<value_type, 0_bi>()
	);
}

} // namespace containers