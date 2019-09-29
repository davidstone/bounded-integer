// Copyright David Stone 2018.
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

template<
	auto lhs_min, auto lhs_max, typename lhs_policy,
	auto rhs_min, auto rhs_max, typename rhs_policy
>
constexpr auto operator&(
	integer<lhs_min, lhs_max, lhs_policy> const lhs_,
	integer<rhs_min, rhs_max, rhs_policy> const rhs_
) {
	// Assume always positive integers for now
	// Not the tightest possible bounds, but probably good enough for now
	return ::bounded::detail::operator_overload(lhs_, rhs_, std::bit_and{}, [](auto const lhs, auto const rhs) {
		return detail::min_max{
			0,
			min(lhs.max, rhs.max).value()
		};
	});
}

}	// namespace bounded

