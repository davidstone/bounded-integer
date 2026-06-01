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

template<typename T>
concept has_member_linear_size = requires {
	bounded::declval<T>().linear_size();
};

export constexpr auto linear_size = []<range Range>(Range && r) -> range_size_t<Range> {
	if constexpr (sized_range<Range>) {
		return containers::size(OPERATORS_FORWARD(r));
	} else if constexpr (has_member_linear_size<Range>) {
		return OPERATORS_FORWARD(r).linear_size();
	} else {
		return ::bounded::assume_in_range<range_size_t<Range>>(containers::distance(
			containers::begin(OPERATORS_FORWARD(r)),
			containers::end(OPERATORS_FORWARD(r))
		));
	}
};

} // namespace containers
