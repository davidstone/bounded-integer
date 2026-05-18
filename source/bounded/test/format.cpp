// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <catch2/catch_test_macros.hpp>
#include <numeric_traits/has_int128.hpp>

export module bounded.test.format;

import bounded.integer;
import bounded.format;

import numeric_traits;
import std_module;

namespace {

TEST_CASE("Format 0", "[format]") {
	CHECK(std::format("{}", bounded::constant<0>) == "0");
}

TEST_CASE("Format 1", "[format]") {
	CHECK(std::format("{}", bounded::constant<1>) == "1");
}

TEST_CASE("Format -1", "[format]") {
	CHECK(std::format("{}", bounded::constant<-1>) == "-1");
}

TEST_CASE("Format large integer", "[format]") {
	CHECK(std::format("{}", bounded::integer(1LL << 60)) == "1152921504606846976");
}

#if defined NUMERIC_TRAITS_HAS_INT128
TEST_CASE("Format very large integer", "[format]") {
	CHECK(std::format("{}", bounded::integer(numeric_traits::max_value<numeric_traits::int128_t>)) == "170141183460469231731687303715884105727");
}
#endif

} // namespace