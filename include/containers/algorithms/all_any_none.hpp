// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/find.hpp>
#include <containers/is_range.hpp>

#include <operators/forward.hpp>
#include <bounded/integer.hpp>

namespace containers {

constexpr auto all(range auto && range, auto predicate) {
	return ::containers::find_if_not(OPERATORS_FORWARD(range), predicate) == end(OPERATORS_FORWARD(range));
}
constexpr auto all_equal(range auto && range, auto && value) {
	return ::containers::all(OPERATORS_FORWARD(range), bounded::equal_to(OPERATORS_FORWARD(value)));
}

constexpr auto any(range auto && range, auto predicate) {
	return ::containers::find_if(OPERATORS_FORWARD(range), predicate) != end(OPERATORS_FORWARD(range));
}
constexpr auto any_equal(range auto && range, auto && value) {
	return ::containers::any(OPERATORS_FORWARD(range), bounded::equal_to(OPERATORS_FORWARD(value)));
}

constexpr auto none(range auto && range, auto predicate) {
	return ::containers::find_if(OPERATORS_FORWARD(range), predicate) == end(OPERATORS_FORWARD(range));
}
constexpr auto none_equal(range auto && range, auto && value) {
	return ::containers::none(OPERATORS_FORWARD(range), bounded::equal_to(OPERATORS_FORWARD(value)));
}

}	// namespace containers
