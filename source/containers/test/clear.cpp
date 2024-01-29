// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.clear;

import containers.clear;
import containers.is_empty;
import containers.vector;

import bounded;
import bounded.test_int;

static_assert([]{
	auto v = containers::vector<bounded_test::non_copyable_integer>({1});
	containers::clear(v);
	BOUNDED_ASSERT(containers::is_empty(v));
	return true;
}());

static_assert([]{
	auto v = containers::vector<bounded_test::non_copyable_integer>();
	containers::clear(v);
	BOUNDED_ASSERT(containers::is_empty(v));
	return true;
}());