// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/is_bounded_integer.hpp>

namespace bounded {

constexpr auto operator<=>(bounded_integer auto const lhs, detail::builtin_integer auto const rhs) {
	return lhs <=> integer(rhs);
}

constexpr auto operator==(bounded_integer auto const lhs, detail::builtin_integer auto const rhs) -> bool {
	return lhs == integer(rhs);
}

constexpr auto operator<=>(bounded_integer auto, bool) = delete;
constexpr auto operator==(bounded_integer auto, bool) -> bool = delete;

}	// namespace bounded
