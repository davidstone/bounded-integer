// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.algorithms.partition;

import containers.algorithms.advance;
import containers.algorithms.find;
import containers.begin_end;
import containers.bidirectional_iterator;
import containers.iterator;
import containers.iterator_t;
import containers.range;
import containers.range_size_t;
import containers.sentinel_for;
import containers.size;

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

export constexpr auto partition_point = []<range Input>(Input && input, auto predicate) -> iterator_t<Input> {
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
	static constexpr auto operator()(ForwardIterator first, sentinel_for<ForwardIterator> auto last, auto predicate) -> ForwardIterator  {
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
	template<range Input>
	static constexpr auto operator()(Input && input, auto predicate) -> iterator_t<Input> {
		return operator()(containers::begin(input), containers::end(input), predicate);
	}
};
export constexpr auto partition = partition_t();

// Partition the range such that all values for which `compare(value, *middle)`
// returns true (prior to any elements moving) appear before the iterator
// returned.
struct iterator_partition_t {
	template<bidirectional_iterator Iterator>
	constexpr auto operator()(Iterator first, Iterator middle, Iterator last, auto const compare) const -> Iterator {
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
