// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <containers/linear_map.hpp>

#include "test_associative_container.hpp"
#include "test_reserve_and_capacity.hpp"
#include "../test_int.hpp"

namespace {

using non_copyable_map = containers::linear_map<bounded_test::non_copyable_integer, bounded_test::non_copyable_integer>;

static_assert(containers_test::test_reserve_and_capacity<non_copyable_map>());
static_assert(containers_test::test_associative_container<non_copyable_map>());

} // namespace
