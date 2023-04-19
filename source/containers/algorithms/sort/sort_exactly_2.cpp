// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.sort_exactly_2;

import containers.algorithms.sort.dereference_all;
import containers.algorithms.sort.relocate_in_order;
import containers.algorithms.sort.sort_test_data;
import containers.algorithms.sort.test_sort_inplace_and_relocate;

import containers.begin_end;

import bounded;
import std_module;

namespace containers {

// stable, 1 compare (average 1), 0-3 relocates (average 1.5)
export constexpr auto sort_exactly_n_impl(auto & x0, auto & x1, auto const compare) -> void {
	if (compare(x1, x0)) {
		std::ranges::swap(x0, x1);
	}
}

export constexpr auto sort_exactly_n(auto it, bounded::constant_t<2> const size, auto const compare) -> void {
	auto [x0, x1, last] = ::containers::dereference_all(std::move(it), size);
	sort_exactly_n_impl(x0, x1, compare);
}

export constexpr auto sort_exactly_n_relocate(auto it, bounded::constant_t<2> const size, auto const out, auto const compare) {
	auto [x0, x1, last] = ::containers::dereference_all(std::move(it), size);
	if (compare(x1, x0)) {
		relocate_in_order(out, x1, x0);
	} else {
		relocate_in_order(out, x0, x1);
	}
	return last;
}

} // namespace containers

using namespace containers_test;
using namespace bounded::literal;

static_assert(test_sort(uint8_2, [](auto & range) {
	containers::sort_exactly_n(containers::begin(range), 2_bi, std::less());
}));
static_assert(test_sort_relocate(uint8_2, [](auto & range, auto & buffer) {
	return containers::sort_exactly_n_relocate(containers::begin(range), 2_bi, buffer.data(), std::less());
}));
