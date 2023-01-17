// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <compare>
#include <catch2/catch_test_macros.hpp>

export module bounded.string_test;

import bounded.integer;
import bounded.string;
import std_module;

namespace {

TEST_CASE("to_string", "[to_string]") {
	auto const result = bounded::to_string(bounded::constant<4>);
	static_assert(std::same_as<decltype(result), std::string const>, "Incorrect type of to_string.");
	CHECK(result == "4");
}

} // namespace