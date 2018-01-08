// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/base.hpp>
#include <bounded/detail/max_builtin.hpp>

namespace bounded {
namespace detail {

constexpr auto left_shift_operator_range = [](auto const lhs, auto const rhs) noexcept {
	// TODO: Broaden range
	return min_max{
		static_cast<max_signed_t>(lhs.min) << rhs.min.value(),
		static_cast<max_signed_t>(lhs.max) << rhs.max.value()
	};
};

}	// namespace detail

BOUNDED_INTEGER_OPERATOR_OVERLOADS(<<, detail::left_shift_operator_range)

}	// namespace bounded

