// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.algorithms.uninitialized;

import containers.algorithms.uninitialized;

import containers.array;
import containers.begin_end;
import containers.subrange;

import bounded;

using namespace bounded::literal;

constexpr auto check_relocate_backward() {
	auto source = containers::array{0_bi, 1_bi, 2_bi, 3_bi};
	bounded::destroy(source[3_bi]);
	containers::uninitialized_relocate_backward(
		containers::subrange(containers::begin(source), containers::begin(source) + 3_bi),
		containers::end(source)
	);
	bounded::construct_at(source[0_bi], [] { return 3_bi; });
	constexpr auto expected = containers::array{3_bi, 0_bi, 1_bi, 2_bi};
	return source == expected;
}
static_assert(check_relocate_backward());
