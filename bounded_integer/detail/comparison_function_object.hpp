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

#include "noexcept.hpp"

#include <utility>

namespace bounded {
namespace detail {

struct binary_equal_to {
	template<typename LHS, typename RHS>
	constexpr decltype(auto) operator()(LHS && lhs, RHS && rhs) const BOUNDED_NOEXCEPT(
		std::forward<LHS>(lhs) == std::forward<RHS>(rhs)
	)
};
template<typename Bound>
struct unary_equal_to {
	constexpr explicit unary_equal_to(Bound && bound) noexcept(std::is_nothrow_constructible<Bound, Bound &&>::value):
		m_bound(std::forward<Bound>(bound))
	{
	}
	template<typename Other>
	constexpr decltype(auto) operator()(Other && other) const BOUNDED_NOEXCEPT(
		m_bound == std::forward<Other>(other)
	)
private:
	Bound m_bound;
};

// TODO: Add total ordering for pointer types
struct less_t {
	template<typename LHS, typename RHS>
	constexpr decltype(auto) operator()(LHS && lhs, RHS && rhs) const BOUNDED_NOEXCEPT(
		std::forward<LHS>(lhs) < std::forward<RHS>(rhs)
	)
};
struct greater_t {
	template<typename LHS, typename RHS>
	constexpr decltype(auto) operator()(LHS && lhs, RHS && rhs) const BOUNDED_NOEXCEPT(
		std::forward<LHS>(lhs) > std::forward<RHS>(rhs)
	)
};

}	// namespace detail

constexpr auto equal_to() noexcept {
	return detail::binary_equal_to{};
}
template<typename T>
constexpr auto equal_to(T && t) BOUNDED_NOEXCEPT(
	detail::unary_equal_to<T>(std::forward<T>(t))
)

constexpr auto less() noexcept {
	return detail::less_t{};
}

constexpr auto greater() noexcept {
	return detail::greater_t{};
}

}	// namespace bounded
