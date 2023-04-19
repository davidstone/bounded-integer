// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <compare>
#include <catch2/catch_test_macros.hpp>

import containers.algorithms.sort.is_sorted;
import containers.algorithms.sort.to_radix_sort_key;

import containers.array;

import bounded.test_int;

namespace {

constexpr auto is_sorted_to_radix(auto... values) {
	auto const original = containers::array{values...};
	CHECK(containers::is_sorted(original));
	auto const converted = containers::array{containers::to_radix_sort_key(values)...};
	return containers::is_sorted(converted);
}

TEST_CASE("to_radix_sort_key", "[to_radix_sort_key]") {
	constexpr int array[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	CHECK(is_sorted_to_radix(
		array + 0,
		array + 1,
		array + 2,
		array + 3,
		array + 4,
		array + 5,
		array + 6,
		array + 7,
		array + 8
	));
}

} // namespace
