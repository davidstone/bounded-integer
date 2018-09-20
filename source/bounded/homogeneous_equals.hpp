// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/class.hpp>

#include <type_traits>

template<typename LHS, typename RHS>
constexpr auto is_same_bounded = std::is_same_v<LHS, RHS>;

template<auto lhs_min, auto lhs_max, auto rhs_min, auto rhs_max, typename policy>
constexpr auto is_same_bounded<
	bounded::integer<lhs_min, lhs_max, policy>,
	bounded::integer<rhs_min, rhs_max, policy>
> =
	bounded::constant<lhs_min> == bounded::constant<rhs_min> and
	bounded::constant<lhs_max> == bounded::constant<rhs_max>;

template<typename Result, typename Expected>
constexpr bool homogeneous_equals(Result const & result, Expected const & expected) noexcept {
	static_assert(is_same_bounded<Result, Expected>, "Mismatched types.");
	return result == expected;
}

