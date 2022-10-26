// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/single_element_range.hpp>

#include <containers/algorithms/compare.hpp>
#include <containers/array.hpp>
#include <containers/front_back.hpp>

#include "../test_assert.hpp"

namespace {
using namespace bounded::literal;

static_assert(std::same_as<decltype(containers::front(containers::single_element_range(std::declval<int const &>()))), int const &>);
static_assert(std::same_as<decltype(containers::front(containers::single_element_range(std::declval<int &&>()))), int &&>);
static_assert(std::same_as<decltype(containers::front(containers::single_element_range(std::declval<int &>()))), int &>);

constexpr auto value = 9;
constexpr auto range = containers::single_element_range(value);

static_assert(containers::begin(range) == containers::begin(range));
static_assert(containers::begin(range) != containers::end(range));
static_assert(containers::begin(range) <=> containers::begin(range) == 0);
static_assert(containers::begin(range) < containers::end(range));

static_assert(containers::begin(range) + 1_bi != containers::begin(range));
static_assert(containers::begin(range) + 1_bi == containers::end(range));
static_assert((containers::begin(range) + 1_bi) - 1_bi == containers::begin(range));

static_assert(containers::begin(range) - containers::begin(range) == 0_bi);
static_assert(containers::end(range) - containers::begin(range) == 1_bi);
static_assert(containers::begin(range) - containers::end(range) == -1_bi);

static_assert(containers::equal(containers::single_element_range(value), containers::array{value}));

} // namespace