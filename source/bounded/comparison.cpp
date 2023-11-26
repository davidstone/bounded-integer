// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.comparison;

import bounded.bounded_integer;
import bounded.builtin_integer;
import bounded.builtin_min_max_value;
import bounded.safe_compare;
import bounded.safe_equal;
import bounded.signed_builtin;

import numeric_traits;
import std_module;

namespace bounded {

export template<bounded_integer LHS, bounded_integer RHS>
constexpr auto operator<=>(LHS const lhs, RHS const rhs) -> std::strong_ordering {
	constexpr auto lhs_min = builtin_min_value<LHS>;
	constexpr auto lhs_max = builtin_max_value<LHS>;
	constexpr auto rhs_min = builtin_min_value<RHS>;
	constexpr auto rhs_max = builtin_max_value<RHS>;
	if constexpr (safe_compare(lhs_min, rhs_max) > 0) {
		return std::strong_ordering::greater;
	} else if constexpr (safe_compare(lhs_max, rhs_min) < 0) {
		return std::strong_ordering::less;
	} else {
		return safe_compare(lhs.value(), rhs.value());
	}
}

export template<bounded_integer LHS, bounded_integer RHS>
constexpr auto operator==(LHS const lhs, RHS const rhs) -> bool {
	constexpr auto lhs_min = builtin_min_value<LHS>;
	constexpr auto lhs_max = builtin_max_value<LHS>;
	constexpr auto rhs_min = builtin_min_value<RHS>;
	constexpr auto rhs_max = builtin_max_value<RHS>;
	if constexpr (safe_compare(lhs_min, rhs_max) > 0 or safe_compare(lhs_max, rhs_min) < 0) {
		return false;
	} else {
		return safe_equal(lhs.value(), rhs.value());
	}
}

export template<typename LHS, typename RHS = LHS>
concept equality_comparable = requires(LHS const & lhs, RHS const & rhs) { lhs == rhs; };

export template<typename LHS, typename RHS = LHS>
concept ordered = requires(LHS const & lhs, RHS const & rhs) { lhs <=> rhs; };

} // namespace bounded
