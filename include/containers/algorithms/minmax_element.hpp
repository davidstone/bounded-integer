// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>

#include <bounded/detail/comparison_function_object.hpp>
#include <bounded/detail/forward.hpp>

namespace containers {

// TODO: minmax_element

constexpr auto min_element(range auto && source, auto compare) {
	auto smallest = begin(BOUNDED_FORWARD(source));
	auto const last = end(BOUNDED_FORWARD(source));
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

constexpr auto min_element(range auto && source) {
	return containers::min_element(BOUNDED_FORWARD(source), bounded::less());
}

constexpr auto max_element(range auto && source, auto greater) {
	auto compare = [cmp = std::move(greater)](auto const & lhs, auto const & rhs) {
		return !(cmp(rhs, lhs));
	};
	return containers::min_element(BOUNDED_FORWARD(source), std::move(compare));
}

constexpr auto max_element(range auto && source) {
	return containers::max_element(BOUNDED_FORWARD(source), bounded::greater());
}

}	// namespace containers
