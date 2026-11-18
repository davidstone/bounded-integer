// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.bitwise_and;

// I would prefer not to implement this, as bounded::integer is not a collection
// of bits. However, many people use x & (y - 1) when they mean x % y, when y is
// a power of two. For instance, gcc's implementation of std::sort

import bounded.bounded_integer;
import bounded.comparison;
import bounded.integer;
import bounded.minmax;
import bounded.normalize;
import bounded.unchecked;

import numeric_traits;
import std_module;

namespace bounded {

export template<bounded_integer LHS, bounded_integer RHS>
constexpr auto operator&(LHS const lhs, RHS const rhs) {
	// Assume always positive integers for now
	// Not the tightest possible bounds, but probably good enough for now
	using result_t = integer<
		0,
		normalize<min(numeric_traits::max_value<LHS>, numeric_traits::max_value<RHS>)>
	>;
	using common_t = typename std::common_type_t<result_t, LHS, RHS>::underlying_type;
	return result_t(
		static_cast<common_t>(lhs) & static_cast<common_t>(rhs),
		unchecked
	);
}

} // namespace bounded

static_assert((bounded::constant<17> & bounded::constant<1>) == bounded::constant<1>);
static_assert((bounded::constant<18> & bounded::constant<1>) == bounded::constant<0>);
