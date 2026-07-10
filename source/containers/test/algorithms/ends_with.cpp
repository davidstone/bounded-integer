// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.algorithms.ends_with;

import containers.algorithms.ends_with;
import containers.static_string;

using namespace containers::string_literals;

static_assert(containers::ends_with(""_s, ""_s));
static_assert(containers::ends_with("a"_s, ""_s));
static_assert(containers::ends_with("a"_s, "a"_s));
static_assert(containers::ends_with("ab"_s, "b"_s));
static_assert(containers::ends_with("ab"_s, "ab"_s));
static_assert(containers::ends_with("aa"_s, "a"_s));
static_assert(containers::ends_with("aa"_s, "aa"_s));
static_assert(!containers::ends_with(""_s, "a"_s));
static_assert(!containers::ends_with("a"_s, "b"_s));
static_assert(!containers::ends_with("a"_s, "aa"_s));
static_assert(!containers::ends_with("ab"_s, "a"_s));