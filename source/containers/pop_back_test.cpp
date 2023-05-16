// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.pop_back_test;

import containers.algorithms.advance;
import containers.algorithms.compare;
import containers.begin_end;
import containers.pop_back;
import containers.range_view;
import containers.vector;

import bounded;
import bounded.test_int;

constexpr bool test_pop_back(containers::vector<bounded_test::integer> const & original) {
	auto copy = original;
	containers::pop_back(copy);
	BOUNDED_ASSERT(containers::equal(
		copy,
		containers::range_view(begin(original), containers::prev(end(original)))
	));
	return true;
}

// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99018
static_assert([] {
	static_assert(test_pop_back({{1}}));
	static_assert(test_pop_back({{1, 2}}));
	return true;
}());
