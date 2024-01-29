// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.push_back;

import containers.push_back;
import containers.vector;

import bounded.test_int;

static_assert([]{
	auto v = containers::vector<bounded_test::integer>();
	containers::push_back(v, 0);
	BOUNDED_ASSERT(v == containers::vector<bounded_test::integer>({0}));
	containers::push_back(v, 1);
	BOUNDED_ASSERT(v == containers::vector<bounded_test::integer>({0, 1}));
	containers::push_back(v, 2);
	BOUNDED_ASSERT(v == containers::vector<bounded_test::integer>({0, 1, 2}));
	containers::push_back(v, 3);
	BOUNDED_ASSERT(v == containers::vector<bounded_test::integer>({0, 1, 2, 3}));
	return true;
}());