// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.count;

import containers.array;
import containers.begin_end;
import containers.count_type;
import containers.range;
import containers.size;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

export template<range Range>
constexpr auto count_if(Range && r, auto predicate) {
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

constexpr auto array = containers::array({0, 3, 2, 3, 5});

static_assert(containers::count(array, 3) == 2_bi);
static_assert(containers::count(array, 2) == 1_bi);
static_assert(containers::count(array, 7) == 0_bi);

constexpr auto false_function = [](auto const &) { return false; };
constexpr auto true_function = [](auto const &) { return true; };

static_assert(containers::count_if(array, false_function) == 0_bi);
static_assert(containers::count_if(array, true_function) == containers::size(array));
