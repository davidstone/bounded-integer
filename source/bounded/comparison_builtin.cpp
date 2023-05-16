// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.comparison_builtin;

export import bounded.comparison;
import bounded.integer;
import bounded.is_bounded_integer;

import std_module;

namespace bounded {

export constexpr auto operator<=>(bounded_integer auto const lhs, builtin_integer auto const rhs) {
	return lhs <=> integer(rhs);
}
export constexpr auto operator==(bounded_integer auto const lhs, builtin_integer auto const rhs) -> bool {
	return lhs == integer(rhs);
}

export constexpr auto operator<=>(bounded_integer auto, bool) = delete;
export constexpr auto operator==(bounded_integer auto, bool) -> bool = delete;

} // namespace bounded
