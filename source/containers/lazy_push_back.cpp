// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/lazy_push_back.hpp>

#include <containers/algorithms/compare.hpp>
#include <containers/vector.hpp>

#include "../test_assert.hpp"

namespace {

using namespace bounded::literal;

constexpr bool test_lazy_push_back() {
    auto c = containers::vector<int>();

    BOUNDED_TEST(c.capacity() == 0_bi);

    lazy_push_back(c, []{ return 3; });
    BOUNDED_TEST(containers::equal(c, containers::vector({3})));

    BOUNDED_TEST(c.capacity() == 1_bi);

    lazy_push_back(c, []{ return 4; });
    BOUNDED_TEST(containers::equal(c, containers::vector({3, 4})));

    BOUNDED_TEST(c.capacity() == 2_bi);

    lazy_push_back(c, []{ return 5; });
    BOUNDED_TEST(containers::equal(c, containers::vector({3, 4, 5})));

    BOUNDED_TEST(c.capacity() == 4_bi);

    lazy_push_back(c, []{ return 12; });
    BOUNDED_TEST(containers::equal(c, containers::vector({3, 4, 5, 12})));

    return true;
}

static_assert(test_lazy_push_back());

} // namespace