// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <catch2/catch_test_macros.hpp>

export module bounded.to_integer_test;

import bounded.to_integer;

import std_module;

TEST_CASE("to_integer", "[to_integer]") {
	CHECK_THROWS_AS((bounded::to_integer<0, 0>("")), std::invalid_argument);
	CHECK_THROWS_AS((bounded::to_integer<-1, 1>("-")), std::invalid_argument);
	CHECK_THROWS_AS((bounded::to_integer<0, 0>("1")), std::invalid_argument);
	CHECK_THROWS_AS((bounded::to_integer<0, 100>("101")), std::invalid_argument);
	CHECK_THROWS_AS((bounded::to_integer<0, 100>("-90")), std::invalid_argument);
}
