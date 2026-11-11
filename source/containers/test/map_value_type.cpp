// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <doctest/doctest.h>

export module containers.test.map_value_type;

import containers.map_value_type;

import bounded;
import std_module;

static_assert(containers::get_key(containers::map_value_type{5, 2}) == 5);
static_assert(containers::get_mapped(containers::map_value_type{5, 2}) == 2);

static_assert(containers::get_key(std::pair{5, 2}) == 5);
static_assert(containers::get_mapped(std::pair{5, 2}) == 2);

TEST_CASE("containers::map_value_type formatting") {
	CHECK(std::format("{}", containers::map_value_type{5, 2}) == "5: 2");
}
