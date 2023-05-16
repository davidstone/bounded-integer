// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.all_any_none;

import containers.algorithms.find;
import containers.array;
import containers.begin_end;
import containers.is_range;
import bounded;

namespace containers {

export constexpr auto all(range auto && range, auto predicate) {
	return ::containers::find_if_not(OPERATORS_FORWARD(range), predicate) == containers::end(OPERATORS_FORWARD(range));
}
export constexpr auto all_equal(range auto && range, auto && value) {
	return ::containers::all(OPERATORS_FORWARD(range), bounded::equal_to(OPERATORS_FORWARD(value)));
}

export constexpr auto any(range auto && range, auto predicate) {
	return ::containers::find_if(OPERATORS_FORWARD(range), predicate) != containers::end(OPERATORS_FORWARD(range));
}
export constexpr auto any_equal(range auto && range, auto && value) {
	return ::containers::any(OPERATORS_FORWARD(range), bounded::equal_to(OPERATORS_FORWARD(value)));
}

export constexpr auto none(range auto && range, auto predicate) {
	return ::containers::find_if(OPERATORS_FORWARD(range), predicate) == containers::end(OPERATORS_FORWARD(range));
}
export constexpr auto none_equal(range auto && range, auto && value) {
	return ::containers::none(OPERATORS_FORWARD(range), bounded::equal_to(OPERATORS_FORWARD(value)));
}

} // namespace containers

static_assert(containers::all_equal(containers::array{2, 2, 2}, 2));
static_assert(!containers::all_equal(containers::array{2, 1, 2}, 2));

static_assert(containers::any_equal(containers::array{2, 2, 2}, 2));
static_assert(containers::any_equal(containers::array{1, 2, 3}, 2));
static_assert(containers::any_equal(containers::array{1, 1, 2}, 2));
static_assert(!containers::any_equal(containers::array{1, 1, 3}, 2));

static_assert(containers::none_equal(containers::array{1, 2, 3}, 4));
static_assert(containers::none_equal(containers::array{1, 1, 1}, 4));
static_assert(!containers::none_equal(containers::array{1, 1, 1, 4}, 4));

