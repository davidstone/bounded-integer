// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/typedefs.hpp>

namespace {

static_assert(bounded::checked_integer<0, 0>(bounded::constant<0>) == bounded::constant<0>);
static_assert(bounded::clamped_integer<0, 0>(bounded::constant<0>) == bounded::constant<0>);
static_assert(bounded::wrapping_integer<0, 0>(bounded::constant<0>) == bounded::constant<0>);

} // namespace
