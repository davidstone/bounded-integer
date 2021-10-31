// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/is_sorted.hpp>
#include <containers/array.hpp>

namespace {

static_assert(containers::is_sorted(containers::array<int, 0>()));
static_assert(containers::is_sorted(containers::array({1})));
static_assert(containers::is_sorted(containers::array({1, 2})));
static_assert(containers::is_sorted(containers::array({1, 1})));
static_assert(!containers::is_sorted(containers::array({2, 1})));
static_assert(containers::is_sorted(containers::array({1, 2, 3})));
static_assert(containers::is_sorted(containers::array({1, 1, 2})));
static_assert(!containers::is_sorted(containers::array({1, 3, 2})));
static_assert(!containers::is_sorted(containers::array({1, 2, 1})));
static_assert(!containers::is_sorted(containers::array({1, 1, 0})));

} // namespace