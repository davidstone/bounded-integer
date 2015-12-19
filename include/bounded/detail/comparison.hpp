// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/common_type.hpp>
#include <bounded/detail/forward_declaration.hpp>
#include <bounded/detail/noexcept.hpp>

#include <cstdint>
#include <type_traits>

namespace bounded {
namespace detail {

// This uses any policy because policies do not matter for comparisons. This
// allows the user to compare integers with unrelated policies.
template<
	intmax_t lhs_min, intmax_t lhs_max, storage_type lhs_storage,
	intmax_t rhs_min, intmax_t rhs_max, storage_type rhs_storage,
	typename policy
>
using comparison_type = typename std::common_type_t<
	integer<lhs_min, lhs_max, policy, lhs_storage, false>,
	integer<rhs_min, rhs_max, policy, rhs_storage, false>
>::underlying_type const;

}	// namespace detail


template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow, storage_type lhs_storage, bool lhs_poisoned,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow, storage_type rhs_storage, bool rhs_poisoned
>
constexpr auto operator==(integer<lhs_min, lhs_max, lhs_overflow, lhs_storage, lhs_poisoned> const & lhs, integer<rhs_min, rhs_max, rhs_overflow, rhs_storage, rhs_poisoned> const & rhs) noexcept {
	if (lhs_min > rhs_max or rhs_min > lhs_max) {
		return false;
	}
	if (lhs_min == lhs_max and rhs_min == rhs_max and lhs_min == rhs_min) {
		return true;
	}
	using common_t = detail::comparison_type<lhs_min, lhs_max, lhs_storage, rhs_min, rhs_max, rhs_storage, lhs_overflow>;
	return static_cast<common_t>(lhs) == static_cast<common_t>(rhs);
}



template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow, storage_type lhs_storage, bool lhs_poisoned,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow, storage_type rhs_storage, bool rhs_poisoned
>
constexpr auto operator<(integer<lhs_min, lhs_max, lhs_overflow, lhs_storage, lhs_poisoned> const & lhs, integer<rhs_min, rhs_max, rhs_overflow, rhs_storage, rhs_poisoned> const & rhs) noexcept {
	if (lhs_min >= rhs_max) {
		return false;
	}
	if (lhs_max < rhs_min) {
		return true;
	}
	using common_t = detail::comparison_type<lhs_min, lhs_max, lhs_storage, rhs_min, rhs_max, rhs_storage, lhs_overflow>;
	return static_cast<common_t>(lhs) < static_cast<common_t>(rhs);
}



// The strange namespacing and using declarations here are to ensure these
// functions are picked up by ADL for types defined in namespaces ::bounded or
// ::bounded::detail
namespace detail {

template<typename LHS, typename RHS>
constexpr auto operator!=(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	!(lhs == rhs)
)

template<typename LHS, typename RHS>
constexpr auto operator>(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	rhs < lhs
)
template<typename LHS, typename RHS>
constexpr auto operator<=(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	!(rhs < lhs)
)
template<typename LHS, typename RHS>
constexpr auto operator>=(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT_DECLTYPE(
	!(lhs < rhs)
)

}	// namespace detail

#define BOUNDED_COMPARISON \
	using ::bounded::detail::operator!=; \
	using ::bounded::detail::operator>; \
	using ::bounded::detail::operator<=; \
	using ::bounded::detail::operator>=;
	
BOUNDED_COMPARISON

}	// namespace bounded
