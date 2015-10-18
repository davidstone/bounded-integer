// ==    !=    <    <=    >    >=
// Copyright (C) 2015 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "common_type.hpp"
#include "forward_declaration.hpp"
#include "noexcept.hpp"

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
	integer<lhs_min, lhs_max, policy, lhs_storage>,
	integer<rhs_min, rhs_max, policy, rhs_storage>
>::underlying_type const;

}	// namespace detail


template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow, storage_type lhs_storage,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow, storage_type rhs_storage
>
constexpr auto operator==(integer<lhs_min, lhs_max, lhs_overflow, lhs_storage> const & lhs, integer<rhs_min, rhs_max, rhs_overflow, rhs_storage> const & rhs) noexcept {
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
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow, storage_type lhs_storage,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow, storage_type rhs_storage
>
constexpr auto operator<(integer<lhs_min, lhs_max, lhs_overflow, lhs_storage> const & lhs, integer<rhs_min, rhs_max, rhs_overflow, rhs_storage> const & rhs) noexcept {
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
constexpr auto operator!=(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT(
	!(lhs == rhs)
)

template<typename LHS, typename RHS>
constexpr auto operator>(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT(
	rhs < lhs
)
template<typename LHS, typename RHS>
constexpr auto operator<=(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT(
	!(rhs < lhs)
)
template<typename LHS, typename RHS>
constexpr auto operator>=(LHS const & lhs, RHS const & rhs) BOUNDED_NOEXCEPT(
	!(lhs < rhs)
)

}	// namespace detail

using ::bounded::detail::operator!=;
using ::bounded::detail::operator>;
using ::bounded::detail::operator<=;
using ::bounded::detail::operator>=;

}	// namespace bounded
