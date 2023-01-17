// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.algorithms.partition;

import containers.algorithms.advance;
import containers.algorithms.find;
import containers.array;
import containers.begin_end;
import containers.is_range;
import containers.is_iterator;
import containers.is_iterator_sentinel;
import containers.size;
import containers.static_vector;

import bounded;
import numeric_traits;
import std_module;

namespace containers {

export constexpr auto is_partitioned = [](range auto && input, auto predicate) -> bool {
	auto first = containers::begin(input);
	auto last = containers::end(input);
	auto it = containers::find_if_not(first, last, predicate);
	return containers::find_if(it, last, predicate) == last;
};

export constexpr auto partition_point = []<range Input>(Input && input, auto predicate) {
	auto count = bounded::integer<0, bounded::builtin_max_value<range_size_t<Input>>>(containers::size(input));
	auto first = containers::begin(input);
	if constexpr (numeric_traits::max_value<decltype(count)> == bounded::constant<0>) {
		return first;
	} else {
		while (count > bounded::constant<0>) {
			auto it = first;
			auto const step = count / bounded::constant<2>;
			::containers::advance(it, step);
			if (predicate(*it)) {
				first = ::containers::next(it);
				count -= step + bounded::constant<1>;
			} else {
				count = step;
			}
		}
		return first;
	}
};

template<typename T>
concept decrementable = requires(T value) { --value; };

struct partition_t {
	template<iterator ForwardIterator>
	static constexpr auto operator()(ForwardIterator first, sentinel_for<ForwardIterator> auto last, auto predicate) {
		auto advance_first = [&]{
			first = containers::find_if_not(first, last, predicate);
		};
		advance_first();

		if constexpr (decrementable<decltype(last)>) {
			auto advance_last = [&]{
				while (first != last) {
					--last;
					if (predicate(*last)) {
						break;
					}
				}
			};

			advance_last();
			while (first != last) {
				std::ranges::swap(*first, *last);
				advance_first();
				if (first == last) {
					break;
				}
				advance_last();
			}
		} else {
			if (first == last) {
				return first;
			}
			for (auto it = containers::next(first); it != last; ++it) {
				if (predicate(*it)) {
					std::ranges::swap(*it, *first);
					++first;
				}
			}
		}
		return first;
	}
	static constexpr auto operator()(range auto && input, auto predicate) {
		return operator()(containers::begin(input), containers::end(input), predicate);
	}
};
export constexpr auto partition = partition_t();

} // namespace containers

using namespace bounded::literal;

constexpr auto predicate = [](bool const value) { return value; };

static_assert(containers::is_partitioned(containers::static_vector<bool, 1_bi>{}, predicate));

static_assert(containers::is_partitioned(containers::array{false}, predicate));
static_assert(containers::is_partitioned(containers::array{true}, predicate));

static_assert(containers::is_partitioned(containers::array{false, false}, predicate));
static_assert(!containers::is_partitioned(containers::array{false, true}, predicate));
static_assert(containers::is_partitioned(containers::array{true, false}, predicate));
static_assert(containers::is_partitioned(containers::array{true, true}, predicate));

static_assert(containers::is_partitioned(containers::array{false, false, false}, predicate));
static_assert(!containers::is_partitioned(containers::array{false, false, true}, predicate));
static_assert(!containers::is_partitioned(containers::array{false, true, false}, predicate));
static_assert(!containers::is_partitioned(containers::array{false, true, true}, predicate));
static_assert(containers::is_partitioned(containers::array{true, false, false}, predicate));
static_assert(!containers::is_partitioned(containers::array{true, false, true}, predicate));
static_assert(containers::is_partitioned(containers::array{true, true, false}, predicate));
static_assert(containers::is_partitioned(containers::array{true, true, true}, predicate));

constexpr auto test_partition_point(auto const & range, auto const expected_offset) {
	BOUNDED_ASSERT(containers::is_partitioned(range, predicate));
	return containers::partition_point(range, predicate) == containers::begin(range) + expected_offset;
}

static_assert(test_partition_point(
	containers::static_vector<bool, 1_bi>{},
	0_bi
));

static_assert(test_partition_point(
	containers::array{false},
	0_bi
));
static_assert(test_partition_point(
	containers::array{true},
	1_bi
));

static_assert(test_partition_point(
	containers::array{false, false},
	0_bi
));
static_assert(test_partition_point(
	containers::array{true, false},
	1_bi
));
static_assert(test_partition_point(
	containers::array{true, true},
	2_bi
));

static_assert(test_partition_point(
	containers::array{false, false, false},
	0_bi
));
static_assert(test_partition_point(
	containers::array{true, false, false},
	1_bi
));
static_assert(test_partition_point(
	containers::array{true, true, false},
	2_bi
));
static_assert(test_partition_point(
	containers::array{true, true, true},
	3_bi
));

constexpr auto test_partition(auto && input, auto const & expected) -> bool {
	BOUNDED_ASSERT(containers::is_partitioned(expected, predicate));
	auto const it = containers::partition(input, predicate);
	BOUNDED_ASSERT(input == expected);
	BOUNDED_ASSERT(it == containers::partition_point(input, predicate));
	return true;
}

static_assert(test_partition(
	containers::static_vector<bool, 1_bi>{},
	containers::static_vector<bool, 1_bi>{}
));

static_assert(test_partition(
	containers::array{false},
	containers::array{false}
));
static_assert(test_partition(
	containers::array{true},
	containers::array{true}
));

static_assert(test_partition(
	containers::array{false, false},
	containers::array{false, false}
));
static_assert(test_partition(
	containers::array{false, true},
	containers::array{true, false}
));
static_assert(test_partition(
	containers::array{true, false},
	containers::array{true, false}
));
static_assert(test_partition(
	containers::array{true, true},
	containers::array{true, true}
));

static_assert(test_partition(
	containers::array{false, false, false},
	containers::array{false, false, false}
));
static_assert(test_partition(
	containers::array{false, false, true},
	containers::array{true, false, false}
));
static_assert(test_partition(
	containers::array{false, true, false},
	containers::array{true, false, false}
));
static_assert(test_partition(
	containers::array{true, false, false},
	containers::array{true, false, false}
));
static_assert(test_partition(
	containers::array{false, true, true},
	containers::array{true, true, false}
));
static_assert(test_partition(
	containers::array{true, false, true},
	containers::array{true, true, false}
));
static_assert(test_partition(
	containers::array{true, true, false},
	containers::array{true, true, false}
));
static_assert(test_partition(
	containers::array{true, true, true},
	containers::array{true, true, true}
));
