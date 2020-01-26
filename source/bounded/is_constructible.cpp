// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/is_constructible.hpp>

static_assert(bounded::constructible_from<void, void>);
static_assert(bounded::constructible_from<int, int>);
static_assert(bounded::constructible_from<void, int>);
static_assert(bounded::constructible_from<int, long>);
static_assert(bounded::constructible_from<long, int>);
static_assert(!bounded::constructible_from<int, void>);
static_assert(bounded::constructible_from<unsigned long long, unsigned long long *>);
