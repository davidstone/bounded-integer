// Functions common to most containers and iterators
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

#include <containers/index_type.hpp>

#include <bounded_integer/bounded_integer.hpp>

#include <utility>

// The strange namespacing and using declarations here are to ensure these
// functions are picked up by ADL for types defined in namespaces ::container or
// ::container::detail

namespace containers {
namespace detail {
namespace common {

template<typename Indexable, typename Index>
constexpr decltype(auto) at(Indexable && indexable, Index const index) BOUNDED_NOEXCEPT(
	std::forward<Indexable>(indexable)[static_cast<index_type<Indexable>>(index)]
)


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

#define CONTAINERS_COMMON_USING_DECLARATIONS \
	using ::containers::detail::common::at; \
	using ::containers::detail::common::operator!=; \
	using ::containers::detail::common::operator>; \
	using ::containers::detail::common::operator<=; \
	using ::containers::detail::common::operator>=;

}	// namespace common

CONTAINERS_COMMON_USING_DECLARATIONS

}	// namespace detail

CONTAINERS_COMMON_USING_DECLARATIONS

#undef CONTAINERS_COMMON_USING_DECLARATIONS

}	// namespace containers
