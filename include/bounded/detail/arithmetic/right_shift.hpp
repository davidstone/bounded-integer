// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>

namespace bounded {
namespace detail {

constexpr auto right_shift_operator_range = [](auto const lhs, auto const rhs) noexcept {
	// TODO: Broaden range
	return min_max(
		static_cast<std::intmax_t>(lhs.min) >> rhs.max.value(),
		static_cast<std::intmax_t>(lhs.max) >> rhs.min.value()
	);
};

}	// namespace detail

BOUNDED_INTEGER_OPERATOR_OVERLOADS(>>, detail::right_shift_operator_range)

}	// namespace bounded

