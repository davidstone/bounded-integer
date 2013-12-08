// Optional class
// Copyright (C) 2013 David Stone
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

#ifndef BOUNDED_INTEGER_DETAIL_OPTIONAL_RELATIONAL_OPERATORS_HPP_
#define BOUNDED_INTEGER_DETAIL_OPTIONAL_RELATIONAL_OPERATORS_HPP_

#include "forward_declaration.hpp"
#include "../class.hpp"

namespace detail {

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator==(optional<lhs_min, lhs_max, lhs_policy> const & lhs, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return (static_cast<bool>(lhs) and static_cast<bool>(rhs)) ?
		*lhs == *rhs :
		(static_cast<bool>(lhs) == static_cast<bool>(rhs));
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator!=(optional<lhs_min, lhs_max, lhs_policy> const & lhs, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(lhs == rhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator<(optional<lhs_min, lhs_max, lhs_policy> const & lhs, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return (static_cast<bool>(lhs) and static_cast<bool>(rhs)) ?
		*lhs < *rhs :
		(static_cast<bool>(lhs) < static_cast<bool>(rhs));
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator>(optional<lhs_min, lhs_max, lhs_policy> const & lhs, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return rhs < lhs;
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator<=(optional<lhs_min, lhs_max, lhs_policy> const & lhs, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(rhs < lhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator>=(optional<lhs_min, lhs_max, lhs_policy> const & lhs, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(lhs < rhs);
}





template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator==(optional<lhs_min, lhs_max, lhs_policy> const & lhs, bounded_integer<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return static_cast<bool>(lhs) and *lhs == rhs;
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator==(bounded_integer<lhs_min, lhs_max, lhs_policy> const & lhs, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return rhs == lhs;
}


template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator!=(optional<lhs_min, lhs_max, lhs_policy> const & lhs, bounded_integer<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(lhs == rhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator!=(bounded_integer<lhs_min, lhs_max, lhs_policy> const & lhs, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(rhs == lhs);
}


template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator<(optional<lhs_min, lhs_max, lhs_policy> const & lhs, bounded_integer<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !static_cast<bool>(lhs) ? true : *lhs == rhs;
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator<(bounded_integer<lhs_min, lhs_max, lhs_policy> const & lhs, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !static_cast<bool>(rhs) ? false : lhs == *rhs;
}


template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator>(optional<lhs_min, lhs_max, lhs_policy> const & lhs, bounded_integer<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return rhs < lhs;
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator>(bounded_integer<lhs_min, lhs_max, lhs_policy> const & lhs, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return rhs < lhs;
}


template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator<=(optional<lhs_min, lhs_max, lhs_policy> const & lhs, bounded_integer<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(rhs < lhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator<=(bounded_integer<lhs_min, lhs_max, lhs_policy> const & lhs, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(rhs < lhs);
}


template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator>=(optional<lhs_min, lhs_max, lhs_policy> const & lhs, bounded_integer<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(lhs < rhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy
>
constexpr bool operator>=(bounded_integer<lhs_min, lhs_max, lhs_policy> const & lhs, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(lhs < rhs);
}






template<intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy>
constexpr bool operator==(optional<lhs_min, lhs_max, lhs_policy> const & lhs, none_t) noexcept {
	return !lhs;
}

template<intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy>
constexpr bool operator==(none_t, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !rhs;
}


template<intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy>
constexpr bool operator!=(optional<lhs_min, lhs_max, lhs_policy> const & lhs, none_t const rhs) noexcept {
	return !(lhs == rhs);
}

template<intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy>
constexpr bool operator!=(none_t const lhs, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(rhs == lhs);
}


template<intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy>
constexpr bool operator<(optional<lhs_min, lhs_max, lhs_policy> const &, none_t) noexcept {
	return false;
}

template<intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy>
constexpr bool operator<(none_t, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return static_cast<bool>(rhs);
}


template<intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy>
constexpr bool operator>(optional<lhs_min, lhs_max, lhs_policy> const & lhs, none_t const rhs) noexcept {
	return rhs < lhs;
}

template<intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy>
constexpr bool operator>(none_t const lhs, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return rhs < lhs;
}


template<intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy>
constexpr bool operator<=(optional<lhs_min, lhs_max, lhs_policy> const & lhs, none_t const rhs) noexcept {
	return !(rhs < lhs);
}

template<intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy>
constexpr bool operator<=(none_t const lhs, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(rhs < lhs);
}


template<intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy>
constexpr bool operator>=(optional<lhs_min, lhs_max, lhs_policy> const & lhs, none_t const rhs) noexcept {
	return !(lhs < rhs);
}

template<intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy>
constexpr bool operator>=(none_t const lhs, optional<rhs_min, rhs_max, rhs_policy> const & rhs) noexcept {
	return !(lhs < rhs);
}

}	// namespace detail

#endif	// BOUNDED_INTEGER_DETAIL_OPTIONAL_RELATIONAL_OPERATORS_HPP_
