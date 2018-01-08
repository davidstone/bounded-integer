// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/abs.hpp>
#include "../homogeneous_equals.hpp"

namespace {

static_assert(homogeneous_equals(
	bounded::abs(bounded::integer<-10, 4>(-5, bounded::non_check)),
	bounded::integer<0, 10>(5, bounded::non_check)
));
static_assert(homogeneous_equals(
	bounded::abs(bounded::integer<-10, -10>(-10, bounded::non_check)),
	bounded::integer<10, 10>(10, bounded::non_check)
));
static_assert(homogeneous_equals(
	bounded::abs(bounded::integer<0, 0>(0, bounded::non_check)),
	bounded::integer<0, 0>(0, bounded::non_check)
));
static_assert(homogeneous_equals(
	bounded::abs(bounded::integer<-7, 450>(-1, bounded::non_check)),
	bounded::integer<0, 450>(1, bounded::non_check)
));
static_assert(homogeneous_equals(
	bounded::abs(bounded::integer<-7, 450>(1, bounded::non_check)),
	bounded::integer<0, 450>(1, bounded::non_check)
));

}	// namespace
