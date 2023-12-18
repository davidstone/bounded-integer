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
import containers.is_iterator;
import containers.range;
import containers.range_size_t;
import containers.sentinel_for;
import containers.size;
import containers.static_vector;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;

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
	if constexpr (numeric_traits::max_value<decltype(count)> == 0_bi) {
		return first;
	} else {
		while (count > 0_bi) {
			auto it = first;
			auto const step = count / 2_bi;
			::containers::advance(it, step);
			if (predicate(*it)) {
				first = ::containers::next(it);
				count -= step + 1_bi;
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

// Partition the range such that all values for which `compare(value, *middle)`
// returns true (prior to any elements moving) appear before the iterator
// returned.
struct iterator_partition_t {
	template<bidirectional_iterator Iterator>
	constexpr auto operator()(Iterator first, Iterator middle, Iterator last, auto const compare) const {
		BOUNDED_ASSERT(first == last or middle != last);
		auto predicate = [&](auto const & value) { return compare(value, *middle); };
		while (true) {
			first = containers::find_if_not(first, middle, predicate);
			if (first == last) {
				return first;
			}
			auto const last_less_than = containers::find_last_if(containers::next(first), last, predicate);
			if (last_less_than == last) {
				return first;
			}
			std::ranges::swap(*first, *last_less_than);
			last = last_less_than;
			if (first == middle) {
				middle = last_less_than;
			}
			++first;
		}
	}
};
export constexpr auto iterator_partition = iterator_partition_t();

} // namespace containers

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

constexpr auto test_iterator_partition(bounded::bounded_integer auto const offset, containers::range auto && input, containers::range auto const & expected) -> bool {
	BOUNDED_ASSERT(containers::is_partitioned(expected, predicate));
	auto const first = containers::begin(input);
	auto const it = containers::iterator_partition(first, first + offset, containers::end(input), std::greater());
	BOUNDED_ASSERT(input == expected);
	BOUNDED_ASSERT(it == containers::partition_point(input, predicate));
	return true;
}

static_assert(test_iterator_partition(
	0_bi,
	containers::static_vector<bool, 1_bi>{},
	containers::static_vector<bool, 1_bi>{}
));

static_assert(test_iterator_partition(
	0_bi,
	containers::array{false},
	containers::array{false}
));

static_assert(test_iterator_partition(
	0_bi,
	containers::array{false, false},
	containers::array{false, false}
));
static_assert(test_iterator_partition(
	1_bi,
	containers::array{false, false},
	containers::array{false, false}
));
static_assert(test_iterator_partition(
	0_bi,
	containers::array{false, true},
	containers::array{true, false}
));
static_assert(test_iterator_partition(
	1_bi,
	containers::array{true, false},
	containers::array{true, false}
));

static_assert(test_iterator_partition(
	0_bi,
	containers::array{false, false, false},
	containers::array{false, false, false}
));
static_assert(test_iterator_partition(
	1_bi,
	containers::array{false, false, false},
	containers::array{false, false, false}
));
static_assert(test_iterator_partition(
	2_bi,
	containers::array{false, false, false},
	containers::array{false, false, false}
));
static_assert(test_iterator_partition(
	0_bi,
	containers::array{false, false, true},
	containers::array{true, false, false}
));
static_assert(test_iterator_partition(
	1_bi,
	containers::array{false, false, true},
	containers::array{true, false, false}
));
static_assert(test_iterator_partition(
	0_bi,
	containers::array{false, true, false},
	containers::array{true, false, false}
));
static_assert(test_iterator_partition(
	2_bi,
	containers::array{false, true, false},
	containers::array{true, false, false}
));
static_assert(test_iterator_partition(
	1_bi,
	containers::array{true, false, false},
	containers::array{true, false, false}
));
static_assert(test_iterator_partition(
	2_bi,
	containers::array{true, false, false},
	containers::array{true, false, false}
));
static_assert(test_iterator_partition(
	0_bi,
	containers::array{false, true, true},
	containers::array{true, true, false}
));
static_assert(test_iterator_partition(
	1_bi,
	containers::array{true, false, true},
	containers::array{true, true, false}
));
static_assert(test_iterator_partition(
	2_bi,
	containers::array{true, true, false},
	containers::array{true, true, false}
));

static_assert(test_iterator_partition(
	0_bi,
	containers::array{false, true, false, true, false, true, false, true, false, true},
	containers::array{true, true, true, true, true, false, false, false, false, false}
));
static_assert(test_iterator_partition(
	2_bi,
	containers::array{false, true, false, true, false, true, false, true, false, true},
	containers::array{true, true, true, true, true, false, false, false, false, false}
));
static_assert(test_iterator_partition(
	4_bi,
	containers::array{false, true, false, true, false, true, false, true, false, true},
	containers::array{true, true, true, true, true, false, false, false, false, false}
));
static_assert(test_iterator_partition(
	6_bi,
	containers::array{false, true, false, true, false, true, false, true, false, true},
	containers::array{true, true, true, true, true, false, false, false, false, false}
));
static_assert(test_iterator_partition(
	8_bi,
	containers::array{false, true, false, true, false, true, false, true, false, true},
	containers::array{true, true, true, true, true, false, false, false, false, false}
));

