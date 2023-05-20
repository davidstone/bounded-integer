// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.sort.sort;

import containers.test.sort.sort_test_data;
import containers.test.sort.test_sort;

import containers.algorithms.sort.sort;

import containers.array;

import bounded;

using namespace containers_test;

static_assert(test_sort(uint8_1, containers::sort));
static_assert(test_sort(uint8_2, containers::sort));
static_assert(test_sort(uint8_3, containers::sort));

static_assert(test_sort(
	containers::array{
		sort_test_data(
			containers::array{0, 1, 2, 3},
			containers::array{0, 1, 2, 3}
		)
	},
	containers::sort
));

static_assert(test_sort(
	containers::array{
		sort_test_data(
			containers::array{
				0,
				1,
				2,
				3,
				4,
				5,
				6,
				7,
				8,
				9,
				10,
				11,
				12,
				13,
				14,
				15,
				16,
				17,
				18,
				19
			},
			containers::array{
				0,
				1,
				2,
				3,
				4,
				5,
				6,
				7,
				8,
				9,
				10,
				11,
				12,
				13,
				14,
				15,
				16,
				17,
				18,
				19
			}
		),
		sort_test_data(
			containers::array{
				19,
				18,
				17,
				16,
				15,
				14,
				13,
				12,
				11,
				10,
				9,
				8,
				7,
				6,
				5,
				4,
				3,
				2,
				1,
				0
			},
			containers::array{
				0,
				1,
				2,
				3,
				4,
				5,
				6,
				7,
				8,
				9,
				10,
				11,
				12,
				13,
				14,
				15,
				16,
				17,
				18,
				19
			}
		),
		sort_test_data(
			containers::array{
				8,
				13,
				17,
				12,
				4,
				0,
				6,
				18,
				3,
				7,
				1,
				11,
				15,
				5,
				10,
				19,
				2,
				16,
				9,
				14
			},
			containers::array{
				0,
				1,
				2,
				3,
				4,
				5,
				6,
				7,
				8,
				9,
				10,
				11,
				12,
				13,
				14,
				15,
				16,
				17,
				18,
				19
			}
		)
	},
	containers::sort
));

static_assert(test_sort(
	containers::array{
		sort_test_data(
			containers::array{
				0,
				1,
				2,
				3,
				4,
				5,
				6,
				7,
				8,
				9,
				10,
				11,
				12,
				13,
				14,
				15,
				16,
				17,
				18,
				19,
				20,
				21,
				22,
				23,
				24,
				25,
				26,
				27,
				28,
				29
			},
			containers::array{
				0,
				1,
				2,
				3,
				4,
				5,
				6,
				7,
				8,
				9,
				10,
				11,
				12,
				13,
				14,
				15,
				16,
				17,
				18,
				19,
				20,
				21,
				22,
				23,
				24,
				25,
				26,
				27,
				28,
				29
			}
		),
		sort_test_data(
			containers::array{
				29,
				28,
				27,
				26,
				25,
				24,
				23,
				22,
				21,
				20,
				19,
				18,
				17,
				16,
				15,
				14,
				13,
				12,
				11,
				10,
				9,
				8,
				7,
				6,
				5,
				4,
				3,
				2,
				1,
				0
			},
			containers::array{
				0,
				1,
				2,
				3,
				4,
				5,
				6,
				7,
				8,
				9,
				10,
				11,
				12,
				13,
				14,
				15,
				16,
				17,
				18,
				19,
				20,
				21,
				22,
				23,
				24,
				25,
				26,
				27,
				28,
				29
			}
		)
	},
	containers::sort
));
static_assert(test_sort(
	containers::array{
		sort_test_data(
			containers::array{
				7,
				16,
				25,
				4,
				6,
				29,
				22,
				12,
				31,
				17,
				33,
				35,
				30,
				3,
				18,
				20,
				19,
				2,
				21,
				39,
				27,
				13,
				38,
				36,
				24,
				26,
				10,
				28,
				23,
				9,
				14,
				5,
				15,
				11,
				8,
				37,
				1,
				32,
				0,
				34
			},
			containers::array{
				0,
				1,
				2,
				3,
				4,
				5,
				6,
				7,
				8,
				9,
				10,
				11,
				12,
				13,
				14,
				15,
				16,
				17,
				18,
				19,
				20,
				21,
				22,
				23,
				24,
				25,
				26,
				27,
				28,
				29,
				30,
				31,
				32,
				33,
				34,
				35,
				36,
				37,
				38,
				39
			}
		)
	},
	containers::sort
));

#if 0

TEST_CASE("sort fuzzer", "[sort]") {
	auto engine = std::mt19937(std::random_device()());
	constexpr auto min_size = 1;
	constexpr auto max_size = 50;
	constexpr auto min_value = -1000;
	constexpr auto max_value = 1000;
	auto size_distribution = std::uniform_int_distribution(min_size, max_size);
	auto value_distribution = std::uniform_int_distribution(min_value, max_value);
	for ([[maybe_unused]] auto const n : containers::integer_range(10000_bi)) {
		auto container = containers::make_static_vector(containers::generate_n(
			bounded::assume_in_range(size_distribution(engine), bounded::constant<min_size>, bounded::constant<max_size>),
			[&] { return value_distribution(engine); }
		));
		containers::sort(container);
		CHECK(containers::is_sorted(container));
	}
}

#endif