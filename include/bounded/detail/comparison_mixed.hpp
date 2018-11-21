// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/requires.hpp>

#include <type_traits>

namespace bounded {
	
template<typename LHS, typename RHS, BOUNDED_REQUIRES(is_bounded_integer<LHS> and detail::is_builtin_integer<RHS>)>
constexpr auto operator<=>(LHS const lhs, RHS const rhs) noexcept {
	return lhs <=> integer(rhs);
}

template<typename LHS, typename RHS, BOUNDED_REQUIRES(detail::is_builtin_integer<LHS> and is_bounded_integer<RHS>)>
constexpr auto operator<=>(LHS const lhs, RHS const rhs) noexcept {
	return integer(lhs) <=> rhs;
}

template<typename LHS, typename RHS, BOUNDED_REQUIRES(is_bounded_integer<LHS> and detail::is_builtin_integer<RHS>)>
constexpr auto operator==(LHS const lhs, RHS const rhs) noexcept {
	return lhs == integer(rhs);
}

template<typename LHS, typename RHS, BOUNDED_REQUIRES(detail::is_builtin_integer<LHS> and is_bounded_integer<RHS>)>
constexpr auto operator==(LHS const lhs, RHS const rhs) noexcept {
	return integer(lhs) == rhs;
}

template<typename LHS, BOUNDED_REQUIRES(is_bounded_integer<LHS>)>
constexpr auto operator<=>(LHS const lhs, bool const rhs) = delete;

template<typename RHS, BOUNDED_REQUIRES(is_bounded_integer<RHS>)>
constexpr auto operator<=>(bool const lhs, RHS const rhs) = delete;

template<typename LHS, BOUNDED_REQUIRES(is_bounded_integer<LHS>)>
constexpr auto operator==(LHS const lhs, bool const rhs) = delete;

template<typename RHS, BOUNDED_REQUIRES(is_bounded_integer<RHS>)>
constexpr auto operator==(bool const lhs, RHS const rhs) = delete;

}	// namespace bounded
