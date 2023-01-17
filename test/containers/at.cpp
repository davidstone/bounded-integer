// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <compare>
#include <catch2/catch_test_macros.hpp>

import containers.array;
import containers.at;

namespace {

TEST_CASE("at throws for out-of-bounds", "[at]") {
	CHECK_THROWS(containers::at(containers::array({1}), 1U));
}

} // namespace