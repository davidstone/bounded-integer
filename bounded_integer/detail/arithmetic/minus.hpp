// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "plus.hpp"
#include "unary_minus.hpp"

#include "../basic_numeric_limits.hpp"
#include "../noexcept.hpp"

namespace bounded {
namespace detail {
namespace arithmetic {

template<typename LHS, typename RHS>
constexpr auto operator-(LHS && lhs, RHS && rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	std::forward<LHS>(lhs) + -std::forward<RHS>(rhs)
)

}	// namespace arithmetic

using bounded::detail::arithmetic::operator-;

}	// namespace detail

using bounded::detail::arithmetic::operator-;

}	// namespace bounded

