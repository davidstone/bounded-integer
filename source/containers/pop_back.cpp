// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/pop_back.hpp>

#include <containers/algorithms/advance.hpp>
#include <containers/static_vector/static_vector.hpp>
#include <containers/range_view.hpp>

#include "../test_assert.hpp"

namespace {

constexpr bool test_pop_back(containers::static_vector<int, 4> const & original) {
    auto copy = original;
    containers::pop_back(copy);
    BOUNDED_TEST(containers::equal(
        copy,
        containers::range_view(begin(original), containers::prev(end(original)))
    ));
    return true;
}

static_assert(test_pop_back({1}));
static_assert(test_pop_back({1, 2}));

} // namespace