// ==    !=    <    <=    >    >=
// Copyright (C) 2014 David Stone
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

#ifndef BOUNDED_INTEGER_COMPARISON_OPERATORS_HPP_
#define BOUNDED_INTEGER_COMPARISON_OPERATORS_HPP_

#include "common_type.hpp"
#include "enable_if.hpp"
#include "forward_declaration.hpp"
#include "make_bounded.hpp"

#include <cstdint>

namespace bounded_integer {

// In my original implementation, I wrote these as comparisons between a
// bounded_integer and its underlying type. However, this actually makes
// relational operators much less useful than they otherwise could be. For
// instance, if we have a bounded_integer with bounds [-100, 100], we might want
// to compare that with an int64_t. If we have to first convert the value to the
// underlying type (likely int8_t), then we are likely to invoke undefined
// behavior. Instead, the comparison function should accept a much wider array
// of types and convert up to the common type.
//
// The common type uses the overflow policy of the left-hand side because the
// policy does not matter for comparisons. This allows the user to compare
// integers with unrelated policies.

namespace detail {

template<typename lhs, typename rhs, typename lhs_overflow>
using comparison_type = typename common_type_t<
	std::decay_t<lhs>,
	equivalent_type<std::decay_t<rhs>, lhs_overflow>
>::underlying_type const;

}	// namespace detail

// Equality
template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow
>
constexpr bool operator==(bounded_integer<lhs_min, lhs_max, lhs_overflow> const lhs, bounded_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	using common_t = detail::comparison_type<decltype(lhs), decltype(rhs), lhs_overflow>;
	return static_cast<common_t>(lhs) == static_cast<common_t>(rhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow
>
constexpr bool operator!=(bounded_integer<lhs_min, lhs_max, lhs_overflow> const lhs, bounded_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	return !(lhs == rhs);
}



// Equality with built-ins

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy,
	typename integer,
	enable_if_t<std::is_integral<integer>::value> = clang_dummy
>
constexpr bool operator==(bounded_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return lhs == make_bounded(rhs);
}

template<intmax_t minimum, intmax_t maximum, typename overflow_policy>
constexpr bool operator==(bounded_integer<minimum, maximum, overflow_policy> const lhs, uintmax_t const rhs) noexcept {
	return (lhs < 0) ? false : static_cast<uintmax_t>(lhs) == rhs;
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy,
	typename integer,
	enable_if_t<std::is_integral<integer>::value> = clang_dummy
>
constexpr bool operator==(integer const lhs, bounded_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return rhs == lhs;
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy,
	typename integer,
	enable_if_t<std::is_integral<integer>::value> = clang_dummy
>
constexpr bool operator!=(bounded_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return !(lhs == rhs);
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy,
	typename integer,
	enable_if_t<std::is_integral<integer>::value> = clang_dummy
>
constexpr bool operator!=(integer const lhs, bounded_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return !(rhs == lhs);
}



// Relational operators

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow
>
constexpr bool operator<(bounded_integer<lhs_min, lhs_max, lhs_overflow> const lhs, bounded_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	using common_t = detail::comparison_type<decltype(lhs), decltype(rhs), lhs_overflow>;
	return static_cast<common_t>(lhs) < static_cast<common_t>(rhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow
>
constexpr bool operator>(bounded_integer<lhs_min, lhs_max, lhs_overflow> const lhs, bounded_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	return rhs < lhs;
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow
>
constexpr bool operator<=(bounded_integer<lhs_min, lhs_max, lhs_overflow> const lhs, bounded_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	return !(rhs < lhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow
>
constexpr bool operator>=(bounded_integer<lhs_min, lhs_max, lhs_overflow> const lhs, bounded_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	return !(lhs < rhs);
}



// Relational operators with built-ins

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy,
	typename integer,
	enable_if_t<std::is_integral<integer>::value> = clang_dummy
>
constexpr bool operator<(bounded_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return lhs < make_bounded(rhs);
}
template<intmax_t minimum, intmax_t maximum, typename overflow_policy>
constexpr bool operator<(bounded_integer<minimum, maximum, overflow_policy> const lhs, uintmax_t const rhs) noexcept {
	return (lhs < 0) ? true : static_cast<uintmax_t>(lhs) < rhs;
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy,
	typename integer,
	enable_if_t<std::is_integral<integer>::value> = clang_dummy
>
constexpr bool operator<(integer const lhs, bounded_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return make_bounded(lhs) < rhs;
}
template<intmax_t minimum, intmax_t maximum, typename overflow_policy>
constexpr bool operator<(uintmax_t const lhs, bounded_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return (rhs < 0) ? false : lhs < static_cast<uintmax_t>(rhs);
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy,
	typename integer,
	enable_if_t<std::is_integral<integer>::value> = clang_dummy
>
constexpr bool operator>(bounded_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return rhs < lhs;
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy,
	typename integer,
	enable_if_t<std::is_integral<integer>::value> = clang_dummy
>
constexpr bool operator>(integer const lhs, bounded_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return rhs < lhs;
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy,
	typename integer,
	enable_if_t<std::is_integral<integer>::value> = clang_dummy
>
constexpr bool operator<=(bounded_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return !(rhs < lhs);
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy,
	typename integer,
	enable_if_t<std::is_integral<integer>::value> = clang_dummy
>
constexpr bool operator<=(integer const lhs, bounded_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return !(rhs < lhs);
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy,
	typename integer,
	enable_if_t<std::is_integral<integer>::value> = clang_dummy
>
constexpr bool operator>=(bounded_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return !(lhs < rhs);
}

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy,
	typename integer,
	enable_if_t<std::is_integral<integer>::value> = clang_dummy
>
constexpr bool operator>=(integer const lhs, bounded_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return !(lhs < rhs);
}

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_COMPARISON_OPERATORS_HPP_
