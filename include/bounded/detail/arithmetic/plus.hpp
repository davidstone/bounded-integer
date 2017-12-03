// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>

namespace bounded {
namespace detail {

constexpr auto plus_operator_range = [](auto const lhs, auto const rhs) noexcept {
	return min_max(
		lhs.min + rhs.min,
		lhs.max + rhs.max
	);
};

}	// namespace detail

BOUNDED_INTEGER_OPERATOR_OVERLOADS(+, detail::plus_operator_range)

}	// namespace bounded

