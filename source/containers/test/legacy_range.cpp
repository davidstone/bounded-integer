// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.legacy_range;

import containers.array;
import containers.begin_end;
import containers.legacy_range;
import containers.subrange;

import bounded;

using namespace bounded::literal;

constexpr auto null_range = containers::subrange<int *>(nullptr, nullptr);
constexpr auto legacy_null_range = containers::legacy_range(null_range);
static_assert(containers::begin(legacy_null_range) == nullptr);
static_assert(containers::end(legacy_null_range) == nullptr);

constexpr auto int_range = containers::array{1, 3, 5};
constexpr auto legacy_int_range = containers::legacy_range(int_range);
static_assert(containers::begin(legacy_int_range) == containers::begin(int_range));
static_assert(containers::end(legacy_int_range) == containers::end(int_range));
