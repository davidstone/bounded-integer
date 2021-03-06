// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/max_builtin.hpp>
#include <bounded/detail/comparison.hpp>

#include <type_traits>

namespace bounded::detail {

template<typename LHS, typename RHS>
constexpr auto safe_min(LHS const lhs, RHS const rhs) {
	using result_t = std::conditional_t<unsigned_builtin<LHS> and unsigned_builtin<RHS>, max_unsigned_t, max_signed_t>;
	return ::bounded::detail::safe_compare(lhs, rhs) <= 0 ?
		static_cast<result_t>(lhs) :
		static_cast<result_t>(rhs);
}

constexpr auto safe_min(auto const arg0, auto const arg1, auto const ... args) {
	return ::bounded::detail::safe_min(
		::bounded::detail::safe_min(arg0, arg1),
		args...
	);
}

template<typename LHS, typename RHS>
constexpr auto safe_max(LHS const lhs, RHS const rhs) {
	using result_t = std::conditional_t<signed_builtin<LHS> and signed_builtin<RHS>, max_signed_t, max_unsigned_t>;
	return ::bounded::detail::safe_compare(lhs, rhs) > 0 ?
		static_cast<result_t>(lhs) :
		static_cast<result_t>(rhs);
}

constexpr auto safe_max(auto const arg0, auto const arg1, auto const ... args) {
	return ::bounded::detail::safe_max(
		::bounded::detail::safe_max(arg0, arg1),
		args...
	);
}

} // namespace bounded::detail