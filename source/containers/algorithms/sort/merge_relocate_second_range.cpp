// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.algorithms.sort.merge_relocate_second_range;

import containers.algorithms.uninitialized;

import containers.array;
import containers.begin_end;
import containers.is_empty;
import containers.iterator;
import containers.range;
import containers.range_view;

import bounded;

using namespace bounded::literal;

namespace containers {

// `r1` is assumed to not need any relocation for any elements at the beginning
// that are before any elements in `r2`. Essentially, overall data layout is
// "contents of r1", then enough space for the contents of r2, with `r2` itself
// being in a separate region of memory.
export constexpr auto merge_relocate_second_range(range auto && r1, range auto && r2, iterator auto out_last, auto const compare) -> void {
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

} // namespace containers

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
