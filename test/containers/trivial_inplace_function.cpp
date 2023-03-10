// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <compare>
#include <catch2/catch_test_macros.hpp>

import containers.trivial_inplace_function;

namespace {

using const_function = containers::trivial_inplace_function<int() const, 24>;
using mutable_function = containers::trivial_inplace_function<int(), 24>;

constexpr auto normal_function() -> int {
	return 2;
}
constexpr auto big_returns_five = const_function([] { return 5; });

TEST_CASE("trivial_inplace_function lambda", "[trivial_inplace_function]") {
	CHECK(big_returns_five() == 5);

	auto increments = mutable_function([x = 0]() mutable { return ++x; });

	CHECK(increments() == 1);
	CHECK(increments() == 2);
}

TEST_CASE("trivial_inplace_function const const function pointer", "[trivial_inplace_function]") {
	auto const f = const_function(normal_function);
	CHECK(f() == 2);
}

TEST_CASE("trivial_inplace_function const mutable function pointer", "[trivial_inplace_function]") {
	auto f = const_function(normal_function);
	CHECK(f() == 2);
}

TEST_CASE("trivial_inplace_function mutable function pointer", "[trivial_inplace_function]") {
	auto f = mutable_function(normal_function);
	CHECK(f() == 2);
}

} // namespace
