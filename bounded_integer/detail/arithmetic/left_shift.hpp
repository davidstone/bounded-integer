// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "base.hpp"

namespace bounded {
namespace detail {

template<typename LHS, typename RHS>
constexpr auto left_shift_operator_range(LHS const & lhs, RHS const & rhs) noexcept {
	return min_max(
		lhs.min << rhs.min,
		lhs.max << rhs.max
	);
}

}	// namespace detail

BOUNDED_INTEGER_OPERATOR_OVERLOADS(<<, detail::left_shift_operator_range)

}	// namespace bounded

