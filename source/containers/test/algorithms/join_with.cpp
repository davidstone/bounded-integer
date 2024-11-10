// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.algorithms.join_with;

import containers.algorithms.compare;
import containers.algorithms.join_with;
import containers.array;
import containers.c_array;

import bounded;
import std_module;

import containers.vector;

using namespace bounded::literal;
using namespace std::string_view_literals;

template<std::size_t size>
constexpr auto compare(
	containers::c_array<std::string_view, size> const & input,
	std::string_view const joiner,
	std::string_view const expected
) -> bool {
	auto rjoined = [&] {
		return containers::join_with(containers::to_array(input), joiner);
	};
	auto const cjoined = rjoined();
	auto mjoined = rjoined();
	return
		containers::equal(cjoined, expected) and
		containers::equal(mjoined, expected) and
		containers::equal(rjoined(), expected);
}

static_assert(containers::equal(
	containers::join_with(containers::array<std::string_view, 0_bi>(), ""sv),
	""sv
));

static_assert(containers::equal(
	containers::join_with(containers::array<std::string_view, 0_bi>(), "|"sv),
	""sv
));

static_assert(compare(
	{""sv},
	""sv,
	""sv
));

static_assert(compare(
	{""sv},
	"|"sv,
	""sv
));

static_assert(compare(
	{""sv, ""sv},
	""sv,
	""sv
));

static_assert(compare(
	{""sv, ""sv},
	"|"sv,
	"|"sv
));

static_assert(compare(
	{"a"sv},
	""sv,
	"a"sv
));

static_assert(compare(
	{"a"sv},
	"|"sv,
	"a"sv
));

static_assert(compare(
	{"a"sv, ""sv},
	""sv,
	"a"sv
));

static_assert(compare(
	{"a"sv, ""sv},
	"|"sv,
	"a|"sv
));

static_assert(compare(
	{""sv, "a"sv},
	""sv,
	"a"sv
));

static_assert(compare(
	{""sv, "a"sv},
	"|"sv,
	"|a"sv
));

static_assert(compare(
	{"a"sv, "b"sv},
	""sv,
	"ab"sv
));

static_assert(compare(
	{"a"sv, "b"sv},
	"|"sv,
	"a|b"sv
));

static_assert(compare(
	{"abc"sv, "def"sv},
	""sv,
	"abcdef"sv
));

static_assert(compare(
	{"abc"sv, "def"sv},
	"|"sv,
	"abc|def"sv
));

static_assert(compare(
	{"abc"sv, "def"sv},
	", "sv,
	"abc, def"sv
));

static_assert(compare(
	{""sv, "a"sv, ""sv},
	""sv,
	"a"sv
));

static_assert(compare(
	{""sv, "a"sv, ""sv},
	"|"sv,
	"|a|"sv
));

static_assert(compare(
	{"a"sv, ""sv, "b"sv},
	""sv,
	"ab"sv
));

static_assert(compare(
	{"a"sv, ""sv, "b"sv},
	"|"sv,
	"a||b"sv
));
