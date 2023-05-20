// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.lazy_push_back;

import containers.algorithms.compare;
import containers.lazy_push_back;
import containers.vector;

import bounded;
import bounded.test_int;

using namespace bounded::literal;

constexpr bool test_lazy_push_back() {
	using vector = containers::vector<bounded_test::integer>;
	auto c = vector();

	BOUNDED_ASSERT(c.capacity() == 0_bi);

	lazy_push_back(c, []{ return 3; });
	BOUNDED_ASSERT(c.capacity() == 1_bi);
	BOUNDED_ASSERT(c == vector({3}));

	lazy_push_back(c, []{ return 4; });
	BOUNDED_ASSERT(c.capacity() == 2_bi);
	BOUNDED_ASSERT(c == vector({3, 4}));

	lazy_push_back(c, []{ return 5; });
	BOUNDED_ASSERT(c.capacity() == 4_bi);
	BOUNDED_ASSERT(c == vector({3, 4, 5}));

	lazy_push_back(c, []{ return 12; });
	BOUNDED_ASSERT(c.capacity() == 4_bi);
	BOUNDED_ASSERT(c == vector({3, 4, 5, 12}));

	return true;
}

static_assert(test_lazy_push_back());
