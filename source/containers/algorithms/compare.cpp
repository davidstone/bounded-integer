// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/compare.hpp>
#include <containers/array/array.hpp>

namespace {

constexpr auto empty = containers::array<int, 0>{};

static_assert(containers::lexicographical_compare_3way(empty, empty) == 0);
static_assert(containers::equal(empty, empty));

constexpr auto one_one = containers::array{1};
constexpr auto one_two = containers::array{2};

static_assert(containers::lexicographical_compare_3way(empty, one_one) < 0);
static_assert(containers::lexicographical_compare_3way(one_one, empty) > 0);
static_assert(!containers::equal(empty, one_one));
static_assert(!containers::equal(one_one, empty));

static_assert(containers::lexicographical_compare_3way(one_one, one_two) < 0);
static_assert(containers::lexicographical_compare_3way(one_two, one_one) > 0);
static_assert(!containers::equal(one_one, one_two));
static_assert(!containers::equal(one_two, one_one));

static_assert(containers::equal(one_one, one_one));
static_assert(containers::equal(one_two, one_two));

constexpr auto two_forward = containers::array{1, 2};
constexpr auto two_reversed = containers::array{2, 1};

static_assert(containers::lexicographical_compare_3way(empty, two_forward) < 0);
static_assert(containers::lexicographical_compare_3way(two_forward, empty) > 0);
static_assert(!containers::equal(empty, two_forward));
static_assert(!containers::equal(two_forward, empty));

static_assert(containers::lexicographical_compare_3way(one_one, two_forward) < 0);
static_assert(containers::lexicographical_compare_3way(two_forward, one_one) > 0);
static_assert(!containers::equal(one_one, two_forward));
static_assert(!containers::equal(two_forward, one_one));

static_assert(containers::lexicographical_compare_3way(one_two, two_forward) > 0);
static_assert(containers::lexicographical_compare_3way(two_forward, one_two) < 0);
static_assert(!containers::equal(one_two, two_forward));
static_assert(!containers::equal(two_forward, one_two));

static_assert(containers::lexicographical_compare_3way(two_forward, two_reversed) < 0);
static_assert(containers::lexicographical_compare_3way(two_reversed, two_forward) > 0);
static_assert(!containers::equal(two_forward, two_reversed));
static_assert(!containers::equal(two_reversed, two_forward));

static_assert(containers::equal(two_forward, two_forward));
static_assert(containers::equal(two_reversed, two_reversed));

constexpr auto nine = containers::array{1, 2, 3, 4, 5, 6, 7, 8, 9};
constexpr auto ten = containers::array{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

static_assert(containers::lexicographical_compare_3way(empty, nine) < 0);
static_assert(containers::lexicographical_compare_3way(nine, empty) > 0);
static_assert(!containers::equal(empty, nine));
static_assert(!containers::equal(nine, empty));

static_assert(containers::lexicographical_compare_3way(one_one, nine) < 0);
static_assert(containers::lexicographical_compare_3way(nine, one_one) > 0);
static_assert(!containers::equal(one_one, nine));
static_assert(!containers::equal(nine, one_one));

static_assert(containers::lexicographical_compare_3way(one_two, nine) > 0);
static_assert(containers::lexicographical_compare_3way(nine, one_two) < 0);
static_assert(!containers::equal(one_two, nine));
static_assert(!containers::equal(nine, one_two));

static_assert(containers::lexicographical_compare_3way(nine, two_reversed) < 0);
static_assert(containers::lexicographical_compare_3way(two_reversed, nine) > 0);
static_assert(!containers::equal(nine, two_reversed));
static_assert(!containers::equal(two_reversed, nine));

static_assert(containers::lexicographical_compare_3way(nine, ten) < 0);
static_assert(containers::lexicographical_compare_3way(ten, nine) > 0);
static_assert(!containers::equal(nine, ten));
static_assert(!containers::equal(ten, nine));

static_assert(containers::equal(nine, nine));
static_assert(containers::equal(ten, ten));

} // namespace
