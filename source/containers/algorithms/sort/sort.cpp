// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.algorithms.sort.sort;

import containers.algorithms.sort.is_sorted;
import containers.algorithms.sort.small_size_optimized_sort;
import containers.algorithms.sort.sort_exactly_3;
import containers.algorithms.sort.sort_exactly_5;
import containers.algorithms.sort.sort_test_data;
import containers.algorithms.sort.test_sort_inplace_and_relocate;

import containers.algorithms.advance;
import containers.algorithms.partition;

import containers.array;
import containers.begin_end;
import containers.is_range;
import containers.legacy_iterator;
import containers.range_view;
import containers.size;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace containers {

constexpr auto heap_sort(range auto && r, auto const compare) -> void {
	auto const first = containers::begin(r);
	auto const last = containers::end(r);
	std::make_heap(make_legacy_iterator(first), make_legacy_iterator(last), compare);
	std::sort_heap(make_legacy_iterator(first), make_legacy_iterator(last), compare);
}

constexpr auto introsort(range auto && r, auto const compare, auto depth) -> void;

template<bounded::bounded_integer Depth>
constexpr auto introsort_impl(Depth const depth) {
	return [=](range auto && r, auto const compare) {
		if (depth == 0_bi) {
			::containers::heap_sort(r, compare);
			return;
		}
		auto const length = containers::size(r);
		auto const first = containers::begin(r);
		auto const last = containers::end(r);
		auto median = first + length / 2_bi;
		auto const before_last = containers::prev(last);
		auto median_of = [&](auto... its) {
			::containers::sort_exactly_n_impl(*its..., compare);
		};
		if (length >= 1000_bi) {
			median_of(first, first + length / 4_bi, median, median + length / 4_bi, before_last);
		} else {
			static_assert(max_small_sort_size >= 3_bi);
			median_of(first, median, before_last);
		}
		median = iterator_partition(first, median, last, compare);
		auto next_depth = Depth(bounded::assume_in_range<Depth>(depth - 1_bi));
		::containers::introsort(range_view(first, median), compare, next_depth);
		::containers::introsort(range_view(median, last), compare, next_depth);
	};
}

inline constexpr auto introsort(range auto && r, auto const compare, auto depth) -> void {
	::containers::small_size_optimized_sort(r, compare, introsort_impl(depth));
}

struct new_sort_t {
	template<range Range>
	constexpr auto operator()(Range & to_sort, auto compare) const -> void {
		if constexpr (numeric_traits::max_value<range_size_t<Range>> >= 2_bi) {
			auto const size = bounded::integer(containers::size(to_sort));
			auto const depth = 2_bi * bounded::log(size, 2_bi);
			introsort(
				to_sort,
				compare,
				bounded::integer<0, bounded::builtin_max_value<decltype(depth)>>(depth)
			);
		}
		BOUNDED_ASSERT(is_sorted(to_sort, compare));
	}
	constexpr auto operator()(range auto & to_sort) const -> void {
		operator()(to_sort, std::less());
	}
};
export constexpr auto new_sort = new_sort_t();

struct sort_t {
	static constexpr auto operator()(range auto & to_sort, auto cmp) -> void {
		std::sort(
			make_legacy_iterator(containers::begin(to_sort)),
			make_legacy_iterator(containers::end(to_sort)),
			cmp
		);
	}
	static constexpr auto operator()(range auto & to_sort) -> void {
		operator()(to_sort, std::less());
	}
};
export constexpr auto sort = sort_t();

} // namespace containers

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