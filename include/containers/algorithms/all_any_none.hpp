// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/find.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

namespace containers {

template<typename Range, typename UnaryPredicate>
constexpr auto all(Range && range, UnaryPredicate p) {
	return ::containers::find_if_not(BOUNDED_FORWARD(range), p) == end(BOUNDED_FORWARD(range));
}
template<typename Range, typename T>
constexpr auto all_equal(Range && range, T && value) {
	return ::containers::all(BOUNDED_FORWARD(range), bounded::equal_to(BOUNDED_FORWARD(value)));
}

template<typename Range, typename UnaryPredicate>
constexpr auto any(Range && range, UnaryPredicate p) {
	return ::containers::find_if(BOUNDED_FORWARD(range), p) != end(BOUNDED_FORWARD(range));
}
template<typename Range, typename T>
constexpr auto any_equal(Range && range, T && value) {
	return ::containers::any(BOUNDED_FORWARD(range), bounded::equal_to(BOUNDED_FORWARD(value)));
}

template<typename Range, typename UnaryPredicate>
constexpr auto none(Range && range, UnaryPredicate p) {
	return ::containers::find_if(BOUNDED_FORWARD(range), p) == end(BOUNDED_FORWARD(range));
}
template<typename Range, typename T>
constexpr auto none_equal(Range && range, T && value) {
	return ::containers::none(BOUNDED_FORWARD(range), bounded::equal_to(BOUNDED_FORWARD(value)));
}

}	// namespace containers
