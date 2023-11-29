// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.algorithms.filter;

import containers.algorithms.compare;
import containers.algorithms.filter;

import containers.array;

import bounded;

using namespace bounded::literal;

// https://github.com/llvm/llvm-project/issues/59513
struct is_even {
	static constexpr auto operator()(auto const integer) {
		return integer % 2_bi == 0_bi;
	}
};

constexpr auto check_filter() {
	constexpr auto source = containers::array{1_bi, 2_bi, 3_bi, 2_bi, 4_bi, 5_bi, 6_bi, 8_bi};
	constexpr auto expected = containers::array{2_bi, 2_bi, 4_bi, 6_bi, 8_bi};
	auto const filtered = containers::filter(source, is_even());
	return containers::equal(filtered, expected);
}

static_assert(check_filter());
