// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.sort_exactly_n;

import containers.algorithms.sort.sort_exactly_n;

import containers.test.sort.sort_test_data;
import containers.test.sort.test_sort;

import containers.array;
import containers.begin_end;

import bounded;
import std_module;

using namespace containers_test;
using namespace bounded::literal;
using namespace containers_test;


static_assert(test_sort(uint8_1, [](auto & range) {
	containers::sort_exactly_n_in_place(containers::begin(range), 1_bi, std::less());
}));
static_assert(test_sort_relocate(uint8_1, [](auto & range, auto & buffer) {
	return containers::sort_exactly_n_relocate(containers::begin(range), 1_bi, buffer.data(), std::less());
}));


static_assert(test_sort(uint8_2, [](auto & range) {
	containers::sort_exactly_n_in_place(containers::begin(range), 2_bi, std::less());
}));
static_assert(test_sort_relocate(uint8_2, [](auto & range, auto & buffer) {
	return containers::sort_exactly_n_relocate(containers::begin(range), 2_bi, buffer.data(), std::less());
}));


static_assert(test_sort(uint8_3, [](auto & range) {
	containers::sort_exactly_n_in_place(containers::begin(range), 3_bi, std::less());
}));
static_assert(test_sort_relocate(uint8_3, [](auto & range, auto & buffer) {
	return containers::sort_exactly_n_relocate(containers::begin(range), 3_bi, buffer.data(), std::less());
}));


constexpr auto inputs4 = containers::array{
	sort_test_data(
		containers::array{0, 1, 2, 3},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{0, 1, 3, 2},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{0, 2, 1, 3},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{0, 2, 3, 1},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{0, 3, 1, 2},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{0, 3, 2, 1},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{1, 0, 2, 3},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{1, 0, 3, 2},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{1, 2, 0, 3},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{1, 2, 3, 0},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{1, 3, 0, 2},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{1, 3, 2, 0},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{2, 0, 1, 3},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{2, 0, 3, 1},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{2, 1, 0, 3},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{2, 1, 3, 0},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{2, 3, 0, 1},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{2, 3, 1, 0},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{3, 0, 1, 2},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{3, 0, 2, 1},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{3, 1, 0, 2},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{3, 1, 2, 0},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{3, 2, 0, 1},
		containers::array{0, 1, 2, 3}
	),
	sort_test_data(
		containers::array{3, 2, 1, 0},
		containers::array{0, 1, 2, 3}
	),

	sort_test_data(
		containers::array{0, 1, 2, 2},
		containers::array{0, 1, 2, 2}
	),
	sort_test_data(
		containers::array{0, 1, 0, 2},
		containers::array{0, 0, 1, 2}
	),
	sort_test_data(
		containers::array{2, 1, 0, 2},
		containers::array{0, 1, 2, 2}
	),

	sort_test_data(
		containers::array{0, 0, 1, 1},
		containers::array{0, 0, 1, 1}
	),
	sort_test_data(
		containers::array{0, 1, 1, 0},
		containers::array{0, 0, 1, 1}
	),
	sort_test_data(
		containers::array{1, 0, 1, 0},
		containers::array{0, 0, 1, 1}
	),

	sort_test_data(
		containers::array{0, 1, 1, 1},
		containers::array{0, 1, 1, 1}
	),
	sort_test_data(
		containers::array{1, 1, 0, 1},
		containers::array{0, 1, 1, 1}
	),
	sort_test_data(
		containers::array{1, 1, 1, 0},
		containers::array{0, 1, 1, 1}
	),

	sort_test_data(
		containers::array{0, 0, 0, 0},
		containers::array{0, 0, 0, 0}
	)
};

static_assert(test_sort(inputs4, [](auto & range) {
	containers::sort_exactly_n_in_place(containers::begin(range), 4_bi, std::less());
}));
static_assert(test_sort_relocate(inputs4, [](auto & range, auto & buffer) {
	return containers::sort_exactly_n_relocate(containers::begin(range), 4_bi, buffer.data(), std::less());
}));


constexpr auto inputs5 = containers::array{
	sort_test_data(
		containers::array{0, 1, 2, 3, 4},
		containers::array{0, 1, 2, 3, 4}
	),
	sort_test_data(
		containers::array{4, 3, 2, 1, 0},
		containers::array{0, 1, 2, 3, 4}
	),
	sort_test_data(
		containers::array{0, 4, 1, 3, 2},
		containers::array{0, 1, 2, 3, 4}
	),

	sort_test_data(
		containers::array{0, 2, 1, 2, 0},
		containers::array{0, 0, 1, 2, 2}
	),

	sort_test_data(
		containers::array{0, 0, 1, 0, 0},
		containers::array{0, 0, 0, 0, 1}
	),

	sort_test_data(
		containers::array{0, 0, 0, 0, 0},
		containers::array{0, 0, 0, 0, 0}
	)
};

static_assert(test_sort(inputs5, [](auto & range) {
	containers::sort_exactly_n_in_place(containers::begin(range), 5_bi, std::less());
}));


constexpr auto inputs6 = containers::array{
	sort_test_data(
		containers::array{0, 1, 2, 3, 4, 5},
		containers::array{0, 1, 2, 3, 4, 5}
	),
	sort_test_data(
		containers::array{5, 4, 3, 2, 1, 0},
		containers::array{0, 1, 2, 3, 4, 5}
	),
	sort_test_data(
		containers::array{0, 4, 1, 5, 3, 2},
		containers::array{0, 1, 2, 3, 4, 5}
	)
};

#if 0
static_assert(test_sort(inputs6, [](auto & range) {
	containers::sort_exactly_n_in_place(containers::begin(range), 6_bi, std::less());
}));
#endif