// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <std_module/prelude.hpp>
#include <catch2/catch_test_macros.hpp>

import containers.size;
import containers.string;
import containers.test_reserve_and_capacity;
import containers.test_sequence_container;
import containers.test_set_size;
import bounded;

namespace {

auto check_equal(std::string_view const input) {
	auto const output = containers::string(input);
	CHECK(output == input);
	CHECK(containers::size(output) == containers::size(input));
}

TEST_CASE("string", "[string]") {
	containers_test::test_sequence_container<containers::string>();
	containers_test::test_reserve_and_capacity<containers::string>();
	containers_test::test_set_size<containers::string>();
	check_equal("012345678901234567890123");
	check_equal("0123456789012345678901234");
}

} // namespace