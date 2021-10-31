// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/algorithms/count.hpp>

#include <containers/array.hpp>

namespace {

using namespace bounded::literal;

constexpr auto array = containers::array({0, 3, 2, 3, 5});

static_assert(containers::count(array, 3) == 2_bi);
static_assert(containers::count(array, 2) == 1_bi);
static_assert(containers::count(array, 7) == 0_bi);

constexpr auto false_function = [](auto const &) { return false; };
constexpr auto true_function = [](auto const &) { return true; };

static_assert(containers::count_if(array, false_function) == 0_bi);
static_assert(containers::count_if(array, true_function) == containers::size(array));

} // namespace