// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/data.hpp>
#include <containers/reserve_if_reservable.hpp>

#include <bounded/detail/construct_destroy.hpp>

#include "../test_assert.hpp"

namespace containers_test {

using namespace bounded::literal;

template<typename Container>
constexpr auto test_add_zero_to_empty() -> void {
	auto const original = Container();
	auto v = original;
	v.set_size(0_bi);
	BOUNDED_TEST(v == original);
}

template<typename Container>
constexpr auto test_add_zero_to_non_empty() -> void {
	auto const original = Container({1});
	auto v = original;
	v.set_size(1_bi);
	BOUNDED_TEST(v == original);
}

template<typename Container>
constexpr auto test_remove_one() -> void {
	auto v = Container({1});
	bounded::destroy(*containers::begin(v));
	v.set_size(0_bi);
	BOUNDED_TEST(v == Container());
}

template<typename Container>
constexpr auto test_add_one() -> void {
	auto v = Container({1});
	bounded::destroy(*containers::begin(v));
	v.set_size(0_bi);
	bounded::construct_at(*containers::data(v), [] { return 5; });
	v.set_size(1_bi);
	BOUNDED_TEST(v == Container({5}));
}

template<typename Container>
constexpr auto test_set_size() -> bool {
	test_add_zero_to_empty<Container>();
	test_add_zero_to_non_empty<Container>();
	test_remove_one<Container>();
	test_add_one<Container>();
	return true;
}

} // namespace containers_test