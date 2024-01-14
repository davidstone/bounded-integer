// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module tv.test.tuple_cat;

import tv.tuple;
import tv.tuple_cat;

static_assert(tv::tuple_cat() == tv::tuple());

static_assert(tv::tuple_cat(tv::tuple()) == tv::tuple());
static_assert(tv::tuple_cat(tv::tuple(1)) == tv::tuple(1));
static_assert(tv::tuple_cat(tv::tuple(1, 2)) == tv::tuple(1, 2));

static_assert(tv::tuple_cat(tv::tuple(), tv::tuple()) == tv::tuple());
static_assert(tv::tuple_cat(tv::tuple(), tv::tuple(1)) == tv::tuple(1));
static_assert(tv::tuple_cat(tv::tuple(1), tv::tuple()) == tv::tuple(1));
static_assert(tv::tuple_cat(tv::tuple(1), tv::tuple(2)) == tv::tuple(1, 2));
static_assert(tv::tuple_cat(tv::tuple(1, 2), tv::tuple(3)) == tv::tuple(1, 2, 3));
static_assert(tv::tuple_cat(tv::tuple(1, 2), tv::tuple(3, 4)) == tv::tuple(1, 2, 3, 4));

static_assert(tv::tuple_cat(tv::tuple(), tv::tuple(), tv::tuple()) == tv::tuple());
static_assert(tv::tuple_cat(tv::tuple(1), tv::tuple(), tv::tuple()) == tv::tuple(1));
static_assert(tv::tuple_cat(tv::tuple(), tv::tuple(1), tv::tuple()) == tv::tuple(1));
static_assert(tv::tuple_cat(tv::tuple(), tv::tuple(), tv::tuple(1)) == tv::tuple(1));
static_assert(tv::tuple_cat(tv::tuple(1), tv::tuple(), tv::tuple(2)) == tv::tuple(1, 2));
static_assert(tv::tuple_cat(tv::tuple(1), tv::tuple(2), tv::tuple(3)) == tv::tuple(1, 2, 3));
static_assert(tv::tuple_cat(tv::tuple(1, 2), tv::tuple(3), tv::tuple()) == tv::tuple(1, 2, 3));
