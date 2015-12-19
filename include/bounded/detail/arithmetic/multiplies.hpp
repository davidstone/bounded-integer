// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>

#include <bounded/detail/minmax.hpp>

namespace bounded {
namespace detail {

template<typename LHS, typename RHS>
constexpr auto multiplies_operator_range(LHS const & lhs, RHS const & rhs) noexcept {
	auto const p0 = lhs.min * rhs.min;
	auto const p1 = lhs.min * rhs.max;
	auto const p2 = lhs.max * rhs.min;
	auto const p3 = lhs.max * rhs.max;
	return min_max(
		min(p0, p1, p2, p3),
		max(p0, p1, p2, p3)
	);
}

}	// namespace detail

BOUNDED_INTEGER_OPERATOR_OVERLOADS(*, detail::multiplies_operator_range)

}	// namespace bounded

