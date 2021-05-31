// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/uninitialized_dynamic_array.hpp>

#include "test_sequence_container.hpp"
#include "../test_int.hpp"

namespace {

using namespace bounded::literal;

template<int max>
using container = containers::uninitialized_dynamic_array<bounded::test_int, bounded::integer<0, max>>;

static_assert(container<0>().capacity() == 0_bi);
static_assert(container<1>().capacity() == 0_bi);

constexpr auto basic_test() -> bool {
    auto a = container<10>(1_bi);
    auto b = container<10>(1_bi);
    BOUNDED_TEST(a.data() != nullptr);
    BOUNDED_TEST(b.data() != nullptr);
    BOUNDED_TEST(a.data() != b.data());
    BOUNDED_TEST(a.capacity() == 1_bi);
    BOUNDED_TEST(b.capacity() == 1_bi);
    return true;
}
static_assert(basic_test());

constexpr auto move_construction() -> bool {
    auto a = container<1>(1_bi);
    auto const original_pointer = a.data();
    auto b = std::move(a);
    BOUNDED_TEST(a.data() == nullptr);
    BOUNDED_TEST(b.data() == original_pointer);
    BOUNDED_TEST(b.capacity() == 1_bi);
    return true;
}
static_assert(move_construction());

static_assert(containers_test::test_self_move_assignment<container<10>>(0_bi));
static_assert(containers_test::test_self_move_assignment<container<10>>(1_bi));
static_assert(containers_test::test_self_move_assignment<container<0>>(0_bi));

static_assert(containers_test::test_self_swap<container<10>>(0_bi));
static_assert(containers_test::test_self_swap<container<10>>(1_bi));
static_assert(containers_test::test_self_swap<container<0>>(0_bi));

} // namespace