// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/string.hpp>

#include <bounded/detail/class.hpp>

#include <catch2/catch_test_macros.hpp>

#include <type_traits>

namespace {

TEST_CASE("to_string", "[to_string]") {
	auto const result = bounded::to_string(bounded::constant<4>);
	static_assert(std::same_as<decltype(result), std::string const>, "Incorrect type of to_string.");
	CHECK(result == "4");
}

} // namespace