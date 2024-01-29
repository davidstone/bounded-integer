// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.algorithms.join;

import containers.algorithms.compare;
import containers.algorithms.join;
import containers.array;
import containers.c_array;

import bounded;
import std_module;

using namespace bounded::literal;
using namespace std::string_view_literals;

template<std::size_t size>
constexpr auto compare(containers::c_array<std::string_view, size> const & input, std::string_view const expected) -> bool {
	return containers::equal(containers::join(containers::to_array(input)), expected);
}

static_assert(containers::equal(
	containers::join(containers::array<std::string_view, 0_bi>()),
	""sv
));

static_assert(compare(
	{""sv},
	""sv
));

static_assert(compare(
	{""sv, ""sv},
	""sv
));

static_assert(compare(
	{"a"sv},
	"a"sv
));

static_assert(compare(
	{"a"sv, ""sv},
	"a"sv
));

static_assert(compare(
	{""sv, "a"sv},
	"a"sv
));

static_assert(compare(
	{"a"sv, "b"sv},
	"ab"sv
));

static_assert(compare(
	{"abc"sv, "def"sv},
	"abcdef"sv
));

static_assert(compare(
	{""sv, "a"sv, ""sv},
	"a"sv
));

static_assert(compare(
	{"a"sv, ""sv, "b"sv},
	"ab"sv
));
