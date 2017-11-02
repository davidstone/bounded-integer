// Copyright David Stone 2017.
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
	
template<typename LHS, typename RHS>
constexpr auto compare_using_member(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	strong_ordering(lhs.compare(rhs))
)

// C-style variadic makes this function always a worse match than the above
template<typename LHS, typename RHS>
constexpr auto compare_using_member(LHS const & lhs, RHS const & rhs, ...) BOUNDED_NOEXCEPT_DECLTYPE(
	strong_ordering(-rhs.compare(lhs))
)

template<typename LHS, typename RHS>
constexpr auto compare(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	compare_using_member(lhs, rhs)
)

template<typename LHS, typename RHS, BOUNDED_REQUIRES(is_bounded_integer<LHS> and std::is_integral<RHS>{})>
constexpr auto compare(LHS const lhs, RHS const rhs) noexcept {
	return compare(lhs, integer(rhs));
}

template<typename LHS, typename RHS, BOUNDED_REQUIRES(std::is_integral<LHS>{} and is_bounded_integer<RHS>)>
constexpr auto compare(LHS const lhs, RHS const rhs) noexcept {
	return compare(integer(lhs), rhs);
}

template<typename LHS, BOUNDED_REQUIRES(is_bounded_integer<LHS>)>
constexpr auto compare(LHS const lhs, std::uintmax_t const rhs) noexcept {
	return (lhs < constant<0>) ? strong_ordering_less : compare(static_cast<std::uintmax_t>(lhs), rhs);
}

template<typename RHS, BOUNDED_REQUIRES(is_bounded_integer<RHS>)>
constexpr auto compare(std::uintmax_t const lhs, RHS const rhs) noexcept {
	return (rhs < constant<0>) ? strong_ordering_greater : compare(lhs, static_cast<std::uintmax_t>(rhs));
}


template<typename LHS, BOUNDED_REQUIRES(is_bounded_integer<LHS>)>
constexpr auto compare(LHS const lhs, bool const rhs) = delete;

template<typename RHS, BOUNDED_REQUIRES(is_bounded_integer<RHS>)>
constexpr auto compare(bool const lhs, RHS const rhs) = delete;

}	// namespace bounded
