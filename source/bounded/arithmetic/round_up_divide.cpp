// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.round_up_divide;

import bounded.arithmetic.operators;
import bounded.integer;
import bounded.is_bounded_integer;

namespace bounded {

export constexpr auto round_up_divide(bounded_integer auto const lhs, bounded_integer auto const rhs) {
	return (lhs + rhs - constant<1>) / rhs;
}

} // namespace bounded

static_assert(bounded::round_up_divide(bounded::constant<1>, bounded::constant<2>) == bounded::constant<1>);
static_assert(bounded::round_up_divide(bounded::constant<1>, bounded::constant<1>) == bounded::constant<1>);
static_assert(bounded::round_up_divide(bounded::constant<5>, bounded::constant<1>) == bounded::constant<5>);
static_assert(bounded::round_up_divide(bounded::constant<6>, bounded::constant<5>) == bounded::constant<2>);
