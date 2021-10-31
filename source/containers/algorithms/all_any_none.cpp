// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/all_any_none.hpp>

#include <containers/array.hpp>

static_assert(containers::all_equal(containers::array{2, 2, 2}, 2));
static_assert(!containers::all_equal(containers::array{2, 1, 2}, 2));

static_assert(containers::any_equal(containers::array{2, 2, 2}, 2));
static_assert(containers::any_equal(containers::array{1, 2, 3}, 2));
static_assert(containers::any_equal(containers::array{1, 1, 2}, 2));
static_assert(!containers::any_equal(containers::array{1, 1, 3}, 2));

static_assert(containers::none_equal(containers::array{1, 2, 3}, 4));
static_assert(containers::none_equal(containers::array{1, 1, 1}, 4));
static_assert(!containers::none_equal(containers::array{1, 1, 1, 4}, 4));

