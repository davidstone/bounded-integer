// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/plus.hpp>
#include <bounded/detail/arithmetic/unary_minus.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/detail/returns.hpp>

namespace bounded {
namespace detail {
namespace arithmetic {

constexpr auto operator-(auto && lhs, auto && rhs) BOUNDED_RETURNS(
	BOUNDED_FORWARD(lhs) + -BOUNDED_FORWARD(rhs)
)

}	// namespace arithmetic

using bounded::detail::arithmetic::operator-;

}	// namespace detail

using bounded::detail::arithmetic::operator-;

}	// namespace bounded

