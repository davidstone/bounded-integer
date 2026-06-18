// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>

import containers.test.test_reserve_and_capacity;
import containers.test.test_sequence_container;
import containers.test.test_set_size;
import containers.size;
import containers.string;
import containers.static_string;
import containers.string_view;

import bounded;
import std_module;

namespace {

using namespace containers::string_literals;

auto check_equal(containers::string_view const input) {
	auto const output = containers::string(input);
	CHECK(output == input);
	CHECK(containers::size(output) == containers::size(input));
}

TEST_CASE("string: string") {
	static_assert(bounded::default_constructible<containers::string>);
	containers_test::test_sequence_container<containers::string>();
	containers_test::test_reserve_and_capacity<containers::string>();
	containers_test::test_set_size<containers::string>();
	check_equal("012345678901234567890123"_sv);
	check_equal("0123456789012345678901234"_sv);
}

TEST_CASE("string format: Empty string") {
	CHECK(std::format("{}", containers::string()) == ""_s);
}

TEST_CASE("string format: Non-empty string") {
	CHECK(std::format("{}", containers::string("hi"_s)) == "hi"_s);
}

} // namespace