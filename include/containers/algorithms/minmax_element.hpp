// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/begin_end.hpp>

#include <bounded/detail/comparison_function_object.hpp>
#include <bounded/detail/forward.hpp>

namespace containers {

// TODO: minmax_element

template<typename Range, typename Compare>
constexpr auto min_element(Range && range, Compare compare) {
	auto smallest = begin(BOUNDED_FORWARD(range));
	auto const last = end(BOUNDED_FORWARD(range));
	if (smallest == last) {
		return smallest;
	}
	for (auto it = containers::next(smallest); it != last; ++it) {
		if (compare(*it, *smallest)) {
			smallest = it;
		}
	}
	return smallest;
}

template<typename Range>
constexpr auto min_element(Range && range) {
	return containers::min_element(BOUNDED_FORWARD(range), bounded::less());
}

template<typename Range, typename Compare>
constexpr auto max_element(Range && range, Compare greater) {
	auto compare = [cmp = std::move(greater)](auto const & lhs, auto const & rhs) {
		return !(cmp(rhs, lhs));
	};
	return containers::min_element(BOUNDED_FORWARD(range), std::move(compare));
}

template<typename Range>
constexpr auto max_element(Range && range) {
	return containers::max_element(BOUNDED_FORWARD(range), bounded::greater());
}

}	// namespace containers
