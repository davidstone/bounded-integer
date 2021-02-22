// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/safe_extreme.hpp>

namespace {

static_assert(bounded::detail::safe_min(2, 6) == 2);
static_assert(bounded::detail::safe_min(7, 1) == 1);
static_assert(bounded::detail::safe_min(5, 5) == 5);
static_assert(bounded::detail::safe_min(0, 1, 2) == 0);
static_assert(bounded::detail::safe_min(1, 1, 2) == 1);
static_assert(bounded::detail::safe_min(9, 4, 9) == 4);
static_assert(bounded::detail::safe_min(9, 4, 3) == 3);
static_assert(bounded::detail::safe_min(9, 9, 9) == 9);

} // namespace
