// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.algorithms.join;

import containers.algorithms.compare;
import containers.algorithms.join;
import containers.array;
import containers.c_array;
import containers.static_string;
import containers.to_array;

import bounded;
import std_module;

using namespace bounded::literal;
using namespace containers::string_literals;

template<std::size_t size>
constexpr auto compare(containers::c_array<std::string_view, size> const & input, std::string_view const expected) -> bool {
	auto rjoined = [&] {
		return containers::join(containers::to_array(input));
	};
	auto const cjoined = rjoined();
	auto mjoined = rjoined();
	return
		containers::equal(cjoined, expected) and
		containers::equal(mjoined, expected) and
		containers::equal(rjoined(), expected);
}

static_assert(containers::equal(
	containers::join(containers::array<std::string_view, 0_bi>()),
	""_s
));

static_assert(compare(
	{""_s},
	""_s
));

static_assert(compare(
	{""_s, ""_s},
	""_s
));

static_assert(compare(
	{"a"_s},
	"a"_s
));

static_assert(compare(
	{"a"_s, ""_s},
	"a"_s
));

static_assert(compare(
	{""_s, "a"_s},
	"a"_s
));

static_assert(compare(
	{"a"_s, "b"_s},
	"ab"_s
));

static_assert(compare(
	{"abc"_s, "def"_s},
	"abcdef"_s
));

static_assert(compare(
	{""_s, "a"_s, ""_s},
	"a"_s
));

static_assert(compare(
	{"a"_s, ""_s, "b"_s},
	"ab"_s
));
