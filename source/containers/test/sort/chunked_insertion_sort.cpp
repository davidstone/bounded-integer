// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.sort.chunked_insertion_sort;

import containers.test.sort.sort_test_data;
import containers.test.sort.test_sort;

import containers.algorithms.sort.chunked_insertion_sort;

import containers.array;

import bounded;
import std_module;

using namespace bounded::literal;
using namespace containers_test;

constexpr auto do_sort = [](auto & range) { containers::chunked_insertion_sort(range, std::less()); };

static_assert(test_sort(
	containers::array{
		sort_test_data(
			containers::array<int, 0_bi>(),
			containers::array<int, 0_bi>()
		)
	},
	do_sort
));
static_assert(test_sort(
	containers::array{
		sort_test_data(
			containers::array{0},
			containers::array{0}
		)
	},
	do_sort
));
static_assert(test_sort(
	containers::array{
		sort_test_data(
			containers::array{0, 1},
			containers::array{0, 1}
		),
		sort_test_data(
			containers::array{1, 0},
			containers::array{0, 1}
		),
		sort_test_data(
			containers::array{0, 0},
			containers::array{0, 0}
		)
	},
	do_sort
));
static_assert(test_sort(
	containers::array{
		sort_test_data(
			containers::array{0, 1, 2},
			containers::array{0, 1, 2}
		)
	},
	do_sort
));
static_assert(test_sort(
	containers::array{
		sort_test_data(
			containers::array{0, 1, 2, 3},
			containers::array{0, 1, 2, 3}
		)
	},
	do_sort
));
static_assert(test_sort(
	containers::array{
		sort_test_data(
			containers::array{0, 1, 2, 3, 4},
			containers::array{0, 1, 2, 3, 4}
		)
	},
	do_sort
));
static_assert(test_sort(
	containers::array{
		sort_test_data(
			containers::array{0, 1, 2, 3, 4, 5, 6},
			containers::array{0, 1, 2, 3, 4, 5, 6}
		)
	},
	do_sort
));
static_assert(test_sort(
	containers::array{
		sort_test_data(
			containers::array{0, 1, 2, 3, 4, 5, 6, 7},
			containers::array{0, 1, 2, 3, 4, 5, 6, 7}
		)
	},
	do_sort
));
static_assert(test_sort(
	containers::array{
		sort_test_data(
			containers::array{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
			containers::array{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
		)
	},
	do_sort
));
static_assert(test_sort(
	containers::array{
		sort_test_data(
			containers::array{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
			containers::array{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
		)
	},
	do_sort
));
static_assert(test_sort(
	containers::array{
		sort_test_data(
			containers::array{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
			containers::array{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}
		)
	},
	do_sort
));
static_assert(test_sort(
	containers::array{
		sort_test_data(
			containers::array{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
			containers::array{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}
		)
	},
	do_sort
));
