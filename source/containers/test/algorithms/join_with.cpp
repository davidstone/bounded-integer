// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.algorithms.join_with;

import containers.algorithms.compare;
import containers.algorithms.join_with;
import containers.array;
import containers.c_array;
import containers.linear_size;
import containers.static_string;
import containers.to_array;

import bounded;
import std_module;

using namespace bounded::literal;
using namespace containers::string_literals;
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
	containers::join_with(containers::array<std::string_view, 0_bi>(), ""_s),
	""_s
));

static_assert(containers::equal(
	containers::join_with(containers::array<std::string_view, 0_bi>(), "|"_s),
	""_s
));

static_assert(compare(
	{""_s},
	""_s,
	""_s
));

static_assert(compare(
	{""_s},
	"|"_s,
	""_s
));

static_assert(compare(
	{""_s, ""_s},
	""_s,
	""_s
));

static_assert(compare(
	{""_s, ""_s},
	"|"_s,
	"|"_s
));

static_assert(compare(
	{"a"_s},
	""_s,
	"a"_s
));

static_assert(compare(
	{"a"_s},
	"|"_s,
	"a"_s
));

static_assert(compare(
	{"a"_s, ""_s},
	""_s,
	"a"_s
));

static_assert(compare(
	{"a"_s, ""_s},
	"|"_s,
	"a|"_s
));

static_assert(compare(
	{""_s, "a"_s},
	""_s,
	"a"_s
));

static_assert(compare(
	{""_s, "a"_s},
	"|"_s,
	"|a"_s
));

static_assert(compare(
	{"a"_s, "b"_s},
	""_s,
	"ab"_s
));

static_assert(compare(
	{"a"_s, "b"_s},
	"|"_s,
	"a|b"_s
));

static_assert(compare(
	{"abc"_s, "def"_s},
	""_s,
	"abcdef"_s
));

static_assert(compare(
	{"abc"_s, "def"_s},
	"|"_s,
	"abc|def"_s
));

static_assert(compare(
	{"abc"_s, "def"_s},
	", "_s,
	"abc, def"_s
));

static_assert(compare(
	{""_s, "a"_s, ""_s},
	""_s,
	"a"_s
));

static_assert(compare(
	{""_s, "a"_s, ""_s},
	"|"_s,
	"|a|"_s
));

static_assert(compare(
	{"a"_s, ""_s, "b"_s},
	""_s,
	"ab"_s
));

static_assert(compare(
	{"a"_s, ""_s, "b"_s},
	"|"_s,
	"a||b"_s
));

static_assert(containers::linear_size(containers::join_with(containers::array{"a"_s, "ab"_s}, "abc"sv)) == 6_bi);
