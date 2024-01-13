// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module tv.test.tuple_size;

import tv.tuple;
import tv.tuple_size;

import bounded;

using namespace bounded::literal;

static_assert(tv::tuple_size<tv::tuple<>> == 0_bi);
static_assert(tv::tuple_size<tv::tuple<int>> == 1_bi);
static_assert(tv::tuple_size<tv::tuple<int, int>> == 2_bi);