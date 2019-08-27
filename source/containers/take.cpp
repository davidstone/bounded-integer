// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/take.hpp>

#include <containers/array/array.hpp>

namespace {

using namespace bounded::literal;

constexpr auto range = containers::array{0, 1, 2, 3, 4, 5, 6, 7};
static_assert(size(range) == 8_bi);
static_assert(size(containers::take(range, 8_bi)) == 8_bi);
static_assert(size(containers::take(range, 4_bi)) == 4_bi);
static_assert(size(containers::take(range, 1_bi)) == 1_bi);
static_assert(size(containers::take(range, 0_bi)) == 0_bi);
static_assert(size(containers::take(range, 12_bi)) == 8_bi);

constexpr auto taken = containers::take(range, 4_bi);
static_assert(taken[3_bi] == 3);

} // namespace