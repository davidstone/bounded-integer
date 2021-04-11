// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/arithmetic/left_shift.hpp>
#include <bounded/detail/class.hpp>
#include "../../homogeneous_equals.hpp"

namespace {

static_assert(homogeneous_equals(
	bounded::integer<0, 2>(bounded::constant<1>) << bounded::integer<0, 60>(bounded::constant<3>),
	bounded::integer<0, bounded::normalize<(2LL << 60LL)>>(bounded::constant<(1 << 3)>)
));

}	// namespace
