// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/find.hpp>
#include <containers/is_range.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

namespace containers {

constexpr auto all(range auto && range, auto predicate) {
	return ::containers::find_if_not(BOUNDED_FORWARD(range), predicate) == end(BOUNDED_FORWARD(range));
}
constexpr auto all_equal(range auto && range, auto && value) {
	return ::containers::all(BOUNDED_FORWARD(range), bounded::equal_to(BOUNDED_FORWARD(value)));
}

constexpr auto any(range auto && range, auto predicate) {
	return ::containers::find_if(BOUNDED_FORWARD(range), predicate) != end(BOUNDED_FORWARD(range));
}
constexpr auto any_equal(range auto && range, auto && value) {
	return ::containers::any(BOUNDED_FORWARD(range), bounded::equal_to(BOUNDED_FORWARD(value)));
}

constexpr auto none(range auto && range, auto predicate) {
	return ::containers::find_if(BOUNDED_FORWARD(range), predicate) == end(BOUNDED_FORWARD(range));
}
constexpr auto none_equal(range auto && range, auto && value) {
	return ::containers::none(BOUNDED_FORWARD(range), bounded::equal_to(BOUNDED_FORWARD(value)));
}

}	// namespace containers
