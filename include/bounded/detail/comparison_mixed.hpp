// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/is_bounded_integer.hpp>

namespace bounded {
	
template<typename LHS, typename RHS> requires(bounded_integer<LHS> and detail::builtin_integer<RHS>)
constexpr auto compare(LHS const lhs, RHS const rhs) noexcept {
	return compare(lhs, integer(rhs));
}

template<typename LHS, typename RHS> requires(detail::builtin_integer<LHS> and bounded_integer<RHS>)
constexpr auto compare(LHS const lhs, RHS const rhs) noexcept {
	return compare(integer(lhs), rhs);
}

template<typename LHS, typename RHS> requires(bounded_integer<LHS> and detail::builtin_integer<RHS>)
constexpr auto operator==(LHS const lhs, RHS const rhs) noexcept {
	return lhs == integer(rhs);
}

template<typename LHS, typename RHS> requires(detail::builtin_integer<LHS> and bounded_integer<RHS>)
constexpr auto operator==(LHS const lhs, RHS const rhs) noexcept {
	return integer(lhs) == rhs;
}

template<typename LHS> requires bounded_integer<LHS>
constexpr auto compare(LHS const lhs, bool const rhs) = delete;

template<typename RHS> requires bounded_integer<RHS>
constexpr auto compare(bool const lhs, RHS const rhs) = delete;

template<typename LHS> requires bounded_integer<LHS>
constexpr auto operator==(LHS const lhs, bool const rhs) = delete;

template<typename RHS> requires bounded_integer<RHS>
constexpr auto operator==(bool const lhs, RHS const rhs) = delete;

}	// namespace bounded
