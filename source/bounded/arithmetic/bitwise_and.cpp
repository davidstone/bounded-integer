// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.bitwise_and;

// I would prefer not to implement this, as bounded::integer is not a collection
// of bits. However, many people use x & (y - 1) when they mean x % y, when y is
// a power of two. For instance, gcc's implementation of std::sort

import bounded.arithmetic.base;
import bounded.comparison;
import bounded.integer;
import bounded.is_bounded_integer;
import bounded.minmax;

import std_module;

namespace bounded {

export constexpr auto operator&(bounded_integer auto const lhs_, bounded_integer auto const rhs_) {
	// Assume always positive integers for now
	// Not the tightest possible bounds, but probably good enough for now
	return ::bounded::detail::operator_overload(lhs_, rhs_, std::bit_and(), [](auto const lhs, auto const rhs) {
		return detail::min_max{
			0,
			min(lhs.max, rhs.max).value()
		};
	});
}

} // namespace bounded

static_assert((bounded::constant<17> & bounded::constant<1>) == bounded::constant<1>);
static_assert((bounded::constant<18> & bounded::constant<1>) == bounded::constant<0>);
