// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/lazy_push_front.hpp>

#include <containers/bidirectional_linked_list.hpp>

#include <bounded/value_to_function.hpp>

#include "../test_assert.hpp"
#include "../test_int.hpp"

namespace {

using namespace bounded::literal;

template<typename Container>
constexpr auto test_lazy_push_front() -> bool {
    auto c = Container();

    containers::lazy_push_front(c, bounded::value_to_function(3));
    BOUNDED_TEST(c == Container({3}));

    containers::lazy_push_front(c, bounded::value_to_function(4));
    BOUNDED_TEST(c == Container({4, 3}));

    containers::lazy_push_front(c, bounded::value_to_function(5));
    BOUNDED_TEST(c == Container({5, 4, 3}));

    containers::lazy_push_front(c, bounded::value_to_function(12));
    BOUNDED_TEST(c == Container({12, 5, 4, 3}));

    return true;
}

static_assert(test_lazy_push_front<containers::bidirectional_linked_list<bounded_test::non_copyable_integer>>());

} // namespace