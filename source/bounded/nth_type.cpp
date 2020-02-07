// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/nth_type.hpp>

#include <type_traits>

namespace {

static_assert(std::is_same_v<bounded::nth_type<0, int>, int>);
static_assert(std::is_same_v<bounded::nth_type<0, int, long>, int>);
static_assert(std::is_same_v<bounded::nth_type<1, int, long>, long>);

}	// namespace
