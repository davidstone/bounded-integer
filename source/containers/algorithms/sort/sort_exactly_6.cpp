// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.sort_exactly_6;

import containers.algorithms.sort.fixed_size_merge_sort;
import containers.algorithms.sort.sort_exactly_5;
import containers.algorithms.sort.sort_test_data;
import containers.algorithms.sort.test_sort_inplace_and_relocate;

import containers.algorithms.advance;

import containers.array;
import containers.begin_end;

import bounded;
import std_module;
import tv;

namespace containers {
using namespace bounded::literal;

constexpr auto insertion_sort_more_than(auto const first, auto const last, auto const size_greater_than, auto const compare) -> void {
	static_assert(size_greater_than > 0_bi);
	::containers::sort_exactly_n(first, size_greater_than, compare);
	auto final_sorted = first + (size_greater_than - 1_bi);
	for (auto it = containers::next(final_sorted); it != last; ++it) {
		if (compare(*it, *final_sorted)) {
			auto storage = ::tv::relocate_into_storage(*it);
			auto empty_spot = it;
			do {
				::bounded::relocate_at(*empty_spot, *final_sorted);
				empty_spot = final_sorted;
				if (empty_spot == first) {
					break;
				}
				--final_sorted;
			} while (compare(storage.value, *final_sorted));
			::bounded::relocate_at(*empty_spot, storage.value);
		}
		final_sorted = it;
	}
}

export constexpr auto sort_exactly_n(auto it, [[maybe_unused]] bounded::constant_t<6> const size, auto const compare) -> void {
	if consteval {
		::containers::insertion_sort_more_than(it, it + size, 5_bi, compare);
	} else {
	#if 0
		constexpr auto large_size = 3_bi;
		::containers::fixed_size_merge_sort(it, size - large_size, large_size, compare);
	#else
		::containers::insertion_sort_more_than(it, it + size, 5_bi, compare);
	#endif
	}
}

} // namespace containers

using namespace containers_test;
using namespace bounded::literal;

constexpr auto inputs = containers::array{
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

static_assert(test_sort(inputs, [](auto & range) {
	containers::sort_exactly_n(containers::begin(range), 6_bi, std::less());
}));
