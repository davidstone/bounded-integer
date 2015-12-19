// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>
#include <bounded/detail/minmax.hpp>

// I would prefer not to implement this, as bounded::integer is not a collection
// of bits. However, many people use x & (y - 1) when they mean x % y, when y is
// a power of two. For instance, gcc's implementation of std::sort

namespace bounded {
namespace detail {

// Assume always positive integers for now
// Not the tightest possible bounds, but probably good enough for now
template<typename LHS, typename RHS>
constexpr auto bitwise_and_operator_range(LHS const & lhs, RHS const & rhs) noexcept {
	return min_max(
		0,
		min(lhs.max, rhs.max)
	);
}

}	// namespace detail

BOUNDED_INTEGER_OPERATOR_OVERLOADS(&, detail::bitwise_and_operator_range)

}	// namespace bounded

