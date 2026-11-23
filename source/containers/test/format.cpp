module;

#include <doctest/doctest.h>

export module containers.test.format;

import containers.format;
import containers.flat_set;
import containers.array;

import bounded;
import containers;
import std_module;

using namespace bounded::literal;
using namespace containers::string_literals;

TEST_CASE("containers: format empty array") {
	CHECK(std::format("{}", containers::array<int, 0_bi>()) == "[]"_sv);
}

TEST_CASE("containers: format non-empty array") {
	CHECK(std::format("{}", containers::array({3})) == "[3]"_sv);
}

TEST_CASE("containers: format empty flat_set") {
	CHECK(std::format("{}", containers::flat_set<int>()) == "[]"_sv);
}

TEST_CASE("containers: format non-empty flat_set") {
	CHECK(std::format("{}", containers::flat_set<int>({2})) == "[2]"_sv);
}
