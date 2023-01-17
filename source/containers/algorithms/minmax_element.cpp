// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.minmax_element;

import containers.algorithms.advance;
import containers.array;
import containers.begin_end;
import containers.is_range;

import bounded;
import std_module;

namespace containers {

// TODO: minmax_element

export constexpr auto min_element(range auto && source, auto compare) {
	auto smallest = containers::begin(OPERATORS_FORWARD(source));
	auto const last = containers::end(OPERATORS_FORWARD(source));
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

export constexpr auto min_element(range auto && source) {
	return containers::min_element(OPERATORS_FORWARD(source), std::less());
}

export constexpr auto max_element(range auto && source, auto greater) {
	auto compare = [cmp = std::move(greater)](auto const & lhs, auto const & rhs) {
		return !(cmp(rhs, lhs));
	};
	return containers::min_element(OPERATORS_FORWARD(source), std::move(compare));
}

export constexpr auto max_element(range auto && source) {
	return containers::max_element(OPERATORS_FORWARD(source), std::greater());
}

} // namespace containers

using namespace bounded::literal;

constexpr auto correct_min_max(auto const & range, auto const min_index, auto const max_index) {
	return
		containers::min_element(range) == begin(range) + min_index and
		containers::max_element(range) == begin(range) + max_index;
}

constexpr auto zero = containers::array<int, 0_bi>{};
static_assert(correct_min_max(zero, 0_bi, 0_bi));


constexpr auto one = containers::array{1};
static_assert(correct_min_max(one, 0_bi, 0_bi));


constexpr auto two_ordered = containers::array{3, 6};
static_assert(containers::min_element(two_ordered) == begin(two_ordered) + 0_bi);
static_assert(containers::max_element(two_ordered) == begin(two_ordered) + 1_bi);
static_assert(correct_min_max(two_ordered, 0_bi, 1_bi));

constexpr auto two_reversed = containers::array{10, 3};
static_assert(correct_min_max(two_reversed, 1_bi, 0_bi));

constexpr auto two_equal = containers::array{2, 2};
static_assert(correct_min_max(two_equal, 0_bi, 1_bi));


constexpr auto three_ordered = containers::array{-1, 2, 4};
static_assert(correct_min_max(three_ordered, 0_bi, 2_bi));

constexpr auto three_reversed = containers::array{8, 5, 3};
static_assert(correct_min_max(three_reversed, 2_bi, 0_bi));

constexpr auto three_mixed = containers::array{8, 9, 5};
static_assert(correct_min_max(three_mixed, 2_bi, 1_bi));

constexpr auto three_equal = containers::array{8, 8, 8};
static_assert(correct_min_max(three_equal, 0_bi, 2_bi));


constexpr auto three_equal_ordered_less = containers::array{1, 1, 2};
static_assert(correct_min_max(three_equal_ordered_less, 0_bi, 2_bi));

constexpr auto three_equal_reversed_less = containers::array{0, -4, -4};
static_assert(correct_min_max(three_equal_reversed_less, 1_bi, 0_bi));

constexpr auto three_equal_mixed_less = containers::array{0, 8, 0};
static_assert(correct_min_max(three_equal_mixed_less, 0_bi, 1_bi));


constexpr auto three_equal_ordered_greater = containers::array{5, 5, 2};
static_assert(correct_min_max(three_equal_ordered_greater, 2_bi, 1_bi));

constexpr auto three_equal_reversed_greater = containers::array{0, 4, 4};
static_assert(correct_min_max(three_equal_reversed_greater, 0_bi, 2_bi));

constexpr auto three_equal_mixed_greater = containers::array{0, -8, 0};
static_assert(correct_min_max(three_equal_mixed_greater, 1_bi, 2_bi));
