// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.resize;

import containers.data;
import containers.resize;
import containers.vector;

import bounded;
import bounded.test_int;

using namespace bounded::literal;

template<typename T>
constexpr auto test_resize_empty_to_zero() -> void {
	auto v = T();
	containers::resize(v, 0_bi);
	BOUNDED_ASSERT(v == T());
}

template<typename T>
constexpr auto test_resize_empty_to_one() -> void {
	auto v = T();
	containers::resize(v, 1_bi);
	BOUNDED_ASSERT(v == T({0}));
}

template<typename T>
constexpr auto test_resize_non_empty_to_zero() -> void {
	auto v = T({1});
	auto ptr = containers::data(v);
	containers::resize(v, 0_bi);
	BOUNDED_ASSERT(v == T());
	BOUNDED_ASSERT(containers::data(v) == ptr);
}

template<typename T>
constexpr auto test_resize_non_empty_to_same_size() -> void {
	auto v = T({1});
	auto ptr = containers::data(v);
	containers::resize(v, 1_bi);
	BOUNDED_ASSERT(v == T({1}));
	BOUNDED_ASSERT(containers::data(v) == ptr);
}

template<typename T>
constexpr auto test_resize_non_empty_to_larger() -> void {
	auto v = T({1});
	containers::resize(v, 2_bi);
	BOUNDED_ASSERT(v == T({1, 0}));
}

template<typename T>
constexpr auto test_resize() -> bool {
	test_resize_empty_to_zero<T>();
	test_resize_empty_to_one<T>();
	test_resize_non_empty_to_zero<T>();
	test_resize_non_empty_to_same_size<T>();
	test_resize_non_empty_to_larger<T>();
	return true;
}

static_assert(test_resize<containers::vector<int>>());
static_assert(test_resize<containers::vector<bounded_test::non_copyable_integer>>());
