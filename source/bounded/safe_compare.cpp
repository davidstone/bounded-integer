// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.safe_compare;

import bounded.builtin_integer;
import bounded.signed_builtin;

import numeric_traits;
import std_module;

namespace bounded {

namespace {

constexpr auto max_signed = numeric_traits::max_value<numeric_traits::max_signed_t>;

} // namespace

export template<builtin_integer LHS, builtin_integer RHS>
constexpr auto safe_compare(LHS const lhs, RHS const rhs) -> std::strong_ordering {
	if constexpr (signed_builtin<LHS> == signed_builtin<RHS>) {
		return lhs <=> rhs;
	} else if constexpr (numeric_traits::max_value<LHS> <= max_signed and numeric_traits::max_value<RHS> <= max_signed) {
		return static_cast<numeric_traits::max_signed_t>(lhs) <=> static_cast<numeric_traits::max_signed_t>(rhs);
	} else if constexpr (signed_builtin<LHS>) {
		return lhs < 0 ? std::strong_ordering::less : static_cast<RHS>(lhs) <=> rhs;
	} else {
		return rhs < 0 ? std::strong_ordering::greater : lhs <=> static_cast<LHS>(rhs);
	}
}

} // namespace bounded
