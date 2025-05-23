// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.count;

import containers.begin_end;
import containers.count_type;
import containers.range;

import bounded;

using namespace bounded::literal;

namespace containers {

export template<range Range>
constexpr auto count_if(Range && r, auto predicate) -> count_type<Range> {
	auto sum = count_type<Range>(0_bi);
	for (decltype(auto) value : OPERATORS_FORWARD(r)) {
		if (predicate(OPERATORS_FORWARD(value))) {
			++sum;
		}
	}
	return sum;
}

export constexpr auto count(range auto && range, auto const & value) {
	return ::containers::count_if(OPERATORS_FORWARD(range), bounded::equal_to(value));
}

} // namespace containers
