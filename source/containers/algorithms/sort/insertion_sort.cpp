// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.algorithms.sort.insertion_sort;

import containers.algorithms.sort.fixed_size_merge_sort;
import containers.algorithms.sort.small_size_optimized_sort;
import containers.algorithms.sort.sort_exactly_1;
import containers.algorithms.sort.sort_exactly_2;
import containers.algorithms.sort.sort_exactly_3;
import containers.algorithms.sort.sort_exactly_4;
import containers.algorithms.sort.sort_exactly_5;
import containers.algorithms.sort.sort_exactly_6;
import containers.algorithms.sort.sort_test_data;
import containers.algorithms.sort.test_sort_inplace_and_relocate;

import containers.algorithms.advance;
import containers.algorithms.find;
import containers.algorithms.uninitialized;

import containers.array;
import containers.begin_end;
import containers.legacy_iterator;
import containers.is_empty;
import containers.is_iterator;
import containers.is_range;
import containers.offset_type;
import containers.range_view;
import containers.size;
import containers.uninitialized_array;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename Iterator>
constexpr auto maybe_sort_exactly_n_relocate(Iterator it, auto const size, auto out, auto const compare) -> Iterator {
	constexpr auto max_size = numeric_traits::max_value<offset_type<Iterator>>;
	if constexpr (size <= max_size) {
		return ::containers::sort_exactly_n_relocate(it, size, out, compare);
	} else {
		std::unreachable();
	}
}

template<typename Iterator>
constexpr auto runtime_sort_exactly_n_relocate(Iterator it, auto const size, auto out, auto const compare) -> Iterator {
	auto do_sort = [&](auto const constant_size) {
		return ::containers::maybe_sort_exactly_n_relocate(it, constant_size, out, compare);
	};
	switch (static_cast<std::size_t>(size)) {
		case 0:
			return it;
		case 1:
			return do_sort(1_bi);
		case 2:
			return do_sort(2_bi);
		case 3:
			return do_sort(3_bi);
		case 4:
			return do_sort(4_bi);
		default:
			std::unreachable();
	}
}

// `r1` is assumed to not need any relocation for any elements at the beginning
// that are before any elements in `r2`. Essentially, overall data layout is
// "contents of r1", then enough space for the contents of r2, with `r2` itself
// being in a separate region of memory.
constexpr auto merge_relocate_second_range(range auto && r1, range auto && r2, iterator auto out_last, auto const compare) -> void {
	// r1: []0 ^1
	// r2: {}4
	BOUNDED_ASSERT(!containers::is_empty(r2));
	auto const first2 = containers::begin(r2);
	auto last2 = containers::end(r2);
	auto const first1 = containers::begin(r1);
	auto last1 = containers::end(r1);
	auto relocate_remainder_of_r2 = [&] {
		::containers::uninitialized_relocate_no_overlap(range_view(first2, last2), first1);
	};
	if (first1 == last1) {
		relocate_remainder_of_r2();
		return;
	}
	--last1;
	--last2;
	while (true) {
		--out_last;
		if (compare(*last2, *last1)) {
			bounded::relocate_at(*out_last, *last1);
			if (first1 == last1) {
				relocate_remainder_of_r2();
				break;
			}
			--last1;
		} else {
			bounded::relocate_at(*out_last, *last2);
			if (first2 == last2) {
				break;
			}
			--last2;
		}
	}
}

export template<typename Compare = std::less<>>
constexpr auto chunked_insertion_sort(range auto && r, Compare const compare = Compare()) -> void {
	constexpr auto chunk_size = 4_bi;
	auto it = containers::begin(r);
	auto const last = containers::end(r);
	#if 1
	auto const initial_sort_size = bounded::min(chunk_size, ::containers::size(r));
	::containers::small_size_optimized_sort(
		range_view(it, initial_sort_size),
		compare,
		[](auto &&, auto) { std::unreachable(); }
	);
	it += initial_sort_size;
	#endif
	auto buffer = containers::uninitialized_array<std::remove_reference_t<decltype(*it)>, chunk_size.value()>();
	while (it != last) {
		auto const count = bounded::min(chunk_size, last - it);
		auto next_it = ::containers::runtime_sort_exactly_n_relocate(it, count, buffer.data(), compare);
		merge_relocate_second_range(
			range_view(containers::begin(r), it),
			range_view(buffer.data(), count),
			next_it,
			compare
		);
		it = next_it;
	}
};

} // namespace containers

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

static_assert([] {
	auto buffer = containers::array({0, 1});
	auto other = containers::array({4});
	::containers::merge_relocate_second_range(
		containers::range_view(containers::begin(buffer), 1_bi),
		other,
		containers::end(buffer),
		std::less()
	);
	return buffer == containers::array({0, 4});
}());

static_assert([] {
	auto buffer = containers::array({0, 1, 2});
	auto other = containers::array({4});
	::containers::merge_relocate_second_range(
		containers::range_view(containers::begin(buffer), 2_bi),
		other,
		containers::end(buffer),
		std::less()
	);
	return buffer == containers::array({0, 1, 4});
}());

static_assert([] {
	auto buffer = containers::array({0, 1, 2, 3});
	auto other = containers::array({4, 5});
	::containers::merge_relocate_second_range(
		containers::range_view(containers::begin(buffer), 2_bi),
		other,
		containers::end(buffer),
		std::less()
	);
	return buffer == containers::array({0, 1, 4, 5});
}());

static_assert([] {
	auto buffer = containers::array({0, 3, 4});
	auto other = containers::array({2});
	::containers::merge_relocate_second_range(
		containers::range_view(containers::begin(buffer), 2_bi),
		other,
		containers::end(buffer),
		std::less()
	);
	return buffer == containers::array({0, 2, 3});
}());

static_assert([] {
	auto buffer = containers::array({0, 1, 2, 3, 5});
	auto other = containers::array({4});
	::containers::merge_relocate_second_range(
		containers::range_view(containers::begin(buffer), 4_bi),
		other,
		containers::end(buffer),
		std::less()
	);
	return buffer == containers::array({0, 1, 2, 3, 4});
}());
