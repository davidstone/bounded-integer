// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/arithmetic/minus.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/typedefs.hpp>
#include "../../homogeneous_equals.hpp"

namespace {

static_assert(homogeneous_equals(
	bounded::constant<0> - bounded::constant<0>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<0> - bounded::constant<1>,
	bounded::constant<-1>
));
#if 0
static_assert(homogeneous_equals(
	unsigned_max - signed_max,
	bounded::constant<static_cast<bounded::detail::unsigned_max_t>(unsigned_max) - static_cast<bounded::detail::unsigned_max_t>(signed_max)>
));
#endif
static_assert(homogeneous_equals(
	bounded::checked_integer<1, 10>(9, bounded::non_check) - bounded::checked_integer<-3, 11>(4, bounded::non_check),
	bounded::checked_integer<-10, 13>(5, bounded::non_check)
));

}	// namespace
