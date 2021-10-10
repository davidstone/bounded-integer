// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/resize.hpp>

#include <containers/data.hpp>
#include <containers/vector.hpp>

#include "../test_assert.hpp"
#include "../test_int.hpp"

namespace {

using namespace bounded::literal;

template<typename T>
constexpr auto test_resize_empty_to_zero() -> void {
	auto v = T();
	containers::resize(v, 0_bi);
	BOUNDED_TEST(v == T());
}

template<typename T>
constexpr auto test_resize_empty_to_one() -> void {
	auto v = T();
	containers::resize(v, 1_bi);
	BOUNDED_TEST(v == T({0}));
}

template<typename T>
constexpr auto test_resize_non_empty_to_zero() -> void {
	auto v = T({1});
	auto ptr = containers::data(v);
	containers::resize(v, 0_bi);
	BOUNDED_TEST(v == T());
	BOUNDED_TEST(containers::data(v) == ptr);
}

template<typename T>
constexpr auto test_resize_non_empty_to_same_size() -> void {
	auto v = T({1});
	auto ptr = containers::data(v);
	containers::resize(v, 1_bi);
	BOUNDED_TEST(v == T({1}));
	BOUNDED_TEST(containers::data(v) == ptr);
}

template<typename T>
constexpr auto test_resize_non_empty_to_larger() -> void {
	auto v = T({1});
	containers::resize(v, 2_bi);
	BOUNDED_TEST(v == T({1, 0}));
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

} // namespace