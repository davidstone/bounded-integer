// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/abs.hpp>
#include "../homogeneous_equals.hpp"

namespace {

static_assert(homogeneous_equals(
	bounded::abs(bounded::integer<-10, 4>(bounded::constant<-5>)),
	bounded::integer<0, 10>(bounded::constant<5>)
));
static_assert(homogeneous_equals(
	bounded::abs(bounded::integer<-10, -10>(bounded::constant<-10>)),
	bounded::integer<10, 10>(bounded::constant<10>)
));
static_assert(homogeneous_equals(
	bounded::abs(bounded::integer<0, 0>(bounded::constant<0>)),
	bounded::integer<0, 0>(bounded::constant<0>)
));
static_assert(homogeneous_equals(
	bounded::abs(bounded::integer<-7, 450>(bounded::constant<-1>)),
	bounded::integer<0, 450>(bounded::constant<1>)
));
static_assert(homogeneous_equals(
	bounded::abs(bounded::integer<-7, 450>(bounded::constant<1>)),
	bounded::integer<0, 450>(bounded::constant<1>)
));

}	// namespace
