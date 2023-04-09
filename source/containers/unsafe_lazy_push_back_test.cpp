// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.unsafe_lazy_push_back_test;

import containers.front_back;
import containers.unsafe_lazy_push_back;
import containers.vector;

import bounded;
import bounded.test_int;

using namespace bounded::literal;

using vector = containers::vector<bounded_test::integer>;

constexpr auto impl(vector & v, int const value, vector const & expected) -> void {
	auto & result = unsafe_lazy_push_back(v, [=]{ return value; });
	BOUNDED_ASSERT(&result == std::addressof(containers::back(v)));
	BOUNDED_ASSERT(v.capacity() == 2_bi);
	BOUNDED_ASSERT(v == expected);
}

constexpr auto test_lazy_push_back() -> bool {
	auto v = vector();
	v.reserve(2_bi);
	impl(v, 3, vector({3}));
	impl(v, 4, vector({3, 4}));
	return true;
}

static_assert(test_lazy_push_back());
