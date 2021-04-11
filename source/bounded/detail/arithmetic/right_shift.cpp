// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/arithmetic/right_shift.hpp>
#include <bounded/detail/class.hpp>
#include "../../homogeneous_equals.hpp"

namespace {

static_assert(homogeneous_equals(
	bounded::constant<100> >> bounded::integer<0, 50>(bounded::constant<1>),
	bounded::integer<0, 100>(bounded::constant<(100 >> 1)>)
));

}	// namespace
