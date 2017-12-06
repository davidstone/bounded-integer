// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/plus.hpp>
#include <bounded/detail/arithmetic/unary_minus.hpp>

#include <bounded/detail/noexcept.hpp>

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

