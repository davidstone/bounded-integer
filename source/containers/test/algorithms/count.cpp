// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.test.algorithms.count;

import containers.algorithms.count;
import containers.array;
import containers.size;

import bounded;

using namespace bounded::literal;

constexpr auto array = containers::array({0, 3, 2, 3, 5});

static_assert(containers::count(array, 3) == 2_bi);
static_assert(containers::count(array, 2) == 1_bi);
static_assert(containers::count(array, 7) == 0_bi);

constexpr auto false_function = [](auto const &) { return false; };
constexpr auto true_function = [](auto const &) { return true; };

static_assert(containers::count_if(array, false_function) == 0_bi);
static_assert(containers::count_if(array, true_function) == containers::size(array));
