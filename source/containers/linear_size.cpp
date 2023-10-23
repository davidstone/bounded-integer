// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.linear_size;

import containers.algorithms.distance;
import containers.begin_end;
import containers.range;
import containers.range_size_t;
import containers.size;
import containers.sized_range;

import bounded;

namespace containers {

export template<range Range>
constexpr auto linear_size(Range && r) {
	if constexpr (sized_range<Range>) {
		return containers::size(OPERATORS_FORWARD(r));
	} else {
		return ::bounded::assume_in_range<range_size_t<Range>>(containers::distance(
			containers::begin(OPERATORS_FORWARD(r)),
			containers::end(OPERATORS_FORWARD(r))
		));
	}
}

} // namespace containers
