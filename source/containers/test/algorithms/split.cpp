// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.test.algorithms.split;

import containers.algorithms.advance;
import containers.algorithms.compare;
import containers.algorithms.split;
import containers.algorithms.zip;
import containers.array;
import containers.is_empty;
import containers.linear_size;
import containers.span;
import containers.static_string;
import containers.string_view;
import containers.subrange;

import bounded;
import std_module;

using namespace bounded::literal;
using namespace containers::string_literals;

constexpr auto compare(containers::string_view const input, containers::span<containers::string_view const> const expected) -> bool {
	auto const split = containers::split(input, '|');
	for (auto const [split_inner, expected_inner] : containers::zip(split, expected)) {
		BOUNDED_ASSERT(containers::string_view(split_inner) == expected_inner);
	}
	return true;
}

static_assert(compare(
	"a"_sv,
	containers::array({"a"_sv})
));

static_assert(compare(
	"a|b"_sv,
	containers::array({"a"_sv, "b"_sv})
));

static_assert(compare(
	"|a"_sv,
	containers::array({""_sv, "a"_sv})
));

static_assert(compare(
	"a|"_sv,
	containers::array({"a"_sv, ""_sv})
));

static_assert(compare(
	""_sv,
	containers::array({""_sv})
));

static_assert(compare(
	"|"_sv,
	containers::array({""_sv, ""_sv})
));

static_assert(compare(
	"|a||b|"_sv,
	containers::array({""_sv, "a"_sv, ""_sv, "b"_sv, ""_sv})
));

constexpr auto range = containers::split("a"_sv, '|');
static_assert(containers::linear_size(range.begin().remainder()) == 1_bi);
static_assert(containers::string_view(range.begin().remainder()) == "a"_sv);
static_assert(containers::string_view(containers::next(range.begin()).remainder()) == ""_sv);


constexpr auto compare_range(containers::string_view const input, containers::span<containers::string_view const> const expected) -> bool {
	auto const split = containers::split_range(input, ":)"_s);
	for (auto const [split_inner, expected_inner] : containers::zip(split, expected)) {
		BOUNDED_ASSERT(containers::string_view(split_inner) == expected_inner);
	}
	return true;
}

static_assert(compare_range(
	"a"_sv,
	containers::array({"a"_sv})
));

static_assert(compare_range(
	"a:)b"_sv,
	containers::array({"a"_sv, "b"_sv})
));

static_assert(compare_range(
	":)a"_sv,
	containers::array({""_sv, "a"_sv})
));

static_assert(compare_range(
	"a:)"_sv,
	containers::array({"a"_sv, ""_sv})
));

static_assert(compare_range(
	""_sv,
	containers::array({""_sv})
));

static_assert(compare_range(
	":)"_sv,
	containers::array({""_sv, ""_sv})
));

static_assert(compare_range(
	":)a:):)b:)"_sv,
	containers::array({""_sv, "a"_sv, ""_sv, "b"_sv, ""_sv})
));

constexpr auto compare_keep_delimiters(containers::string_view const input, containers::span<containers::string_view const> const expected) -> bool {
	auto const split = containers::split_keep_delimiters(input, '|');
	for (auto const [split_inner, expected_inner] : containers::zip(split, expected)) {
		BOUNDED_ASSERT(containers::string_view(split_inner) == expected_inner);
	}
	return true;
}

static_assert(compare_keep_delimiters(
	"a"_sv,
	containers::array({"a"_sv})
));

static_assert(compare_keep_delimiters(
	"a|b"_sv,
	containers::array({"a"_sv, "|"_sv, "b"_sv})
));

static_assert(compare_keep_delimiters(
	"|a"_sv,
	containers::array({""_sv, "|"_sv, "a"_sv})
));

static_assert(compare_keep_delimiters(
	"a|"_sv,
	containers::array({"a"_sv, "|"_sv, ""_sv})
));

static_assert(compare_keep_delimiters(
	""_sv,
	containers::array({""_sv})
));

static_assert(compare_keep_delimiters(
	"|"_sv,
	containers::array({""_sv, "|"_sv, ""_sv})
));

static_assert(compare_keep_delimiters(
	"|a||b|"_sv,
	containers::array({""_sv, "|"_sv, "a"_sv, "|"_sv, ""_sv, "|"_sv, "b"_sv, "|"_sv, ""_sv})
));


constexpr auto compare_range_keep_delimiters(containers::string_view const input, containers::span<containers::string_view const> const expected) -> bool {
	auto const split = containers::split_range_keep_delimiters(input, ":)"_s);
	for (auto const [split_inner, expected_inner] : containers::zip(split, expected)) {
		BOUNDED_ASSERT(containers::string_view(split_inner) == expected_inner);
	}
	return true;
}

static_assert(compare_range_keep_delimiters(
	"a"_sv,
	containers::array({"a"_sv})
));

static_assert(compare_range_keep_delimiters(
	"a:)b"_sv,
	containers::array({"a"_sv, ":)"_sv, "b"_sv})
));

static_assert(compare_range_keep_delimiters(
	":)a"_sv,
	containers::array({""_sv, ":)"_sv, "a"_sv})
));

static_assert(compare_range_keep_delimiters(
	"a:)"_sv,
	containers::array({"a"_sv, ":)"_sv, ""_sv})
));

static_assert(compare_range_keep_delimiters(
	""_sv,
	containers::array({""_sv})
));

static_assert(compare_range_keep_delimiters(
	":)"_sv,
	containers::array({""_sv, ":)"_sv, ""_sv})
));

static_assert(compare_range_keep_delimiters(
	":)a:):)b:)"_sv,
	containers::array({""_sv, ":)"_sv, "a"_sv, ":)"_sv, ""_sv, ":)"_sv, "b"_sv, ":)"_sv, ""_sv})
));

