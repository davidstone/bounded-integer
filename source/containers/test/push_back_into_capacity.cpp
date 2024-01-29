// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>
#include <operators/forward.hpp>

export module containers.test.push_back_into_capacity;

import containers.back;
import containers.push_back_into_capacity;
import containers.vector;

import bounded;
import bounded.test_int;

using namespace bounded::literal;

using vector = containers::vector<bounded_test::integer>;

constexpr auto impl(vector & v, auto && value, vector const & expected) -> void {
	auto & result = push_back_into_capacity(v, OPERATORS_FORWARD(value));
	BOUNDED_ASSERT(&result == std::addressof(containers::back(v)));
	BOUNDED_ASSERT(v.capacity() == 2_bi);
	BOUNDED_ASSERT(v == expected);
}

static_assert([]{
	auto v = vector();
	v.reserve(2_bi);
	impl(v, 3, vector({3}));
	impl(v, 4, vector({3, 4}));
	return true;
}());

static_assert([]{
	auto v = vector();
	v.reserve(2_bi);
	auto const three = bounded_test::integer(3);
	impl(v, three, vector({3}));
	auto const four = bounded_test::integer(4);
	impl(v, four, vector({3, 4}));
	return true;
}());
