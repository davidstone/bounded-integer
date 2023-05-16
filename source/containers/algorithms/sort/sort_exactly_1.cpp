// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.sort_exactly_1;

import containers.algorithms.sort.sort_test_data;
import containers.algorithms.sort.test_sort_inplace_and_relocate;

import containers.begin_end;

import bounded;
import std_module;

namespace containers {

export constexpr auto sort_exactly_n([[maybe_unused]] auto const it, bounded::constant_t<1>, [[maybe_unused]] auto const compare) -> void {
}

export constexpr auto sort_exactly_n_relocate(auto it, bounded::constant_t<1>, auto const out, [[maybe_unused]] auto const compare) {
	bounded::relocate_at(*out, *it);
	++it;
	return it;
}

} // namespace containers

using namespace containers_test;
using namespace bounded::literal;

static_assert(test_sort(uint8_1, [](auto & range) {
	containers::sort_exactly_n(containers::begin(range), 1_bi, std::less());
}));
static_assert(test_sort_relocate(uint8_1, [](auto & range, auto & buffer) {
	return containers::sort_exactly_n_relocate(containers::begin(range), 1_bi, buffer.data(), std::less());
}));
