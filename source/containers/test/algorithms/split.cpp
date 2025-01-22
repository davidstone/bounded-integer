// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <std_module/prelude.hpp>
#include <string_view>

#include <bounded/assert.hpp>

export module containers.test.algorithms.split;

import containers.algorithms.advance;
import containers.algorithms.compare;
import containers.algorithms.split;
import containers.algorithms.zip;
import containers.array;
import containers.is_empty;
import containers.linear_size;
import containers.subrange;

import bounded;
import std_module;

using namespace bounded::literal;
using namespace std::string_view_literals;

using namespace bounded::literal;
using namespace std::string_view_literals;

constexpr auto compare(std::string_view const input, std::span<std::string_view const> const expected) -> bool {
	auto const split = containers::split(input, '|');
	for (auto const [split_inner, expected_inner] : containers::zip(split, expected)) {
		BOUNDED_ASSERT(std::string_view(split_inner) == expected_inner);
	}
	return true;
}

static_assert(compare(
	"a"sv,
	containers::array({"a"sv})
));

static_assert(compare(
	"a|b"sv,
	containers::array({"a"sv, "b"sv})
));

static_assert(compare(
	"|a"sv,
	containers::array({""sv, "a"sv})
));

static_assert(compare(
	"a|"sv,
	containers::array({"a"sv, ""sv})
));

static_assert(compare(
	""sv,
	containers::array({""sv})
));

static_assert(compare(
	"|"sv,
	containers::array({""sv, ""sv})
));

static_assert(compare(
	"|a||b|"sv,
	containers::array({""sv, "a"sv, ""sv, "b"sv, ""sv})
));

constexpr auto range = containers::split("a"sv, '|');
static_assert(containers::linear_size(range.begin().remainder()) == 1_bi);
static_assert(std::string_view(range.begin().remainder()) == "a");
static_assert(std::string_view(containers::next(range.begin()).remainder()) == "");


constexpr auto compare_range(std::string_view const input, std::span<std::string_view const> const expected) -> bool {
	auto const split = containers::split_range(input, ":)");
	for (auto const [split_inner, expected_inner] : containers::zip(split, expected)) {
		BOUNDED_ASSERT(std::string_view(split_inner) == expected_inner);
	}
	return true;
}

static_assert(compare_range(
	"a"sv,
	containers::array({"a"sv})
));

static_assert(compare_range(
	"a:)b"sv,
	containers::array({"a"sv, "b"sv})
));

static_assert(compare_range(
	":)a"sv,
	containers::array({""sv, "a"sv})
));

static_assert(compare_range(
	"a:)"sv,
	containers::array({"a"sv, ""sv})
));

static_assert(compare_range(
	""sv,
	containers::array({""sv})
));

static_assert(compare_range(
	":)"sv,
	containers::array({""sv, ""sv})
));

static_assert(compare_range(
	":)a:):)b:)"sv,
	containers::array({""sv, "a"sv, ""sv, "b"sv, ""sv})
));

