// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.safe_extreme;

import bounded.comparison;
import bounded.safe_compare;
import bounded.signed_builtin;

import numeric_traits;
import std_module;

namespace bounded {

export template<typename LHS, typename RHS>
constexpr auto safe_min(LHS const lhs, RHS const rhs) {
	using result_t = std::conditional_t<numeric_traits::unsigned_builtin<LHS> and numeric_traits::unsigned_builtin<RHS>, numeric_traits::max_unsigned_t, numeric_traits::max_signed_t>;
	return safe_compare(lhs, rhs) <= 0 ?
		static_cast<result_t>(lhs) :
		static_cast<result_t>(rhs);
}

export constexpr auto safe_min(auto const arg0, auto const arg1, auto const ... args) {
	return safe_min(
		safe_min(arg0, arg1),
		args...
	);
}

export template<typename LHS, typename RHS>
constexpr auto safe_max(LHS const lhs, RHS const rhs) {
	using result_t = std::conditional_t<numeric_traits::signed_builtin<LHS> and numeric_traits::signed_builtin<RHS>, numeric_traits::max_signed_t, numeric_traits::max_unsigned_t>;
	return safe_compare(lhs, rhs) > 0 ?
		static_cast<result_t>(lhs) :
		static_cast<result_t>(rhs);
}

export constexpr auto safe_max(auto const arg0, auto const arg1, auto const ... args) {
	return safe_max(
		safe_max(arg0, arg1),
		args...
	);
}

} // namespace bounded

namespace {

static_assert(bounded::safe_min(2, 6) == 2);
static_assert(bounded::safe_min(7, 1) == 1);
static_assert(bounded::safe_min(5, 5) == 5);
static_assert(bounded::safe_min(0, 1, 2) == 0);
static_assert(bounded::safe_min(1, 1, 2) == 1);
static_assert(bounded::safe_min(9, 4, 9) == 4);
static_assert(bounded::safe_min(9, 4, 3) == 3);
static_assert(bounded::safe_min(9, 9, 9) == 9);

} // namespace
