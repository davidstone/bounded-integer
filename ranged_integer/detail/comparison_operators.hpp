// ==    !=    <    <=    >    >=
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

#ifndef RANGED_INTEGER_COMPARISON_OPERATORS_HPP_
#define RANGED_INTEGER_COMPARISON_OPERATORS_HPP_

#include "common_type.hpp"
#include "enable_if.hpp"
#include "forward_declaration.hpp"
#include "make_ranged.hpp"

#include <cstdint>

// In my original implementation, I wrote these as comparisons between a
// ranged_integer and its underlying type. However, this actually makes
// relational operators much less useful than they otherwise could be. For
// instance, if we have a ranged_integer with bounds [-100, 100], we might want
// to compare that with an int64_t. If we have to first convert the value to the
// underlying type (likely int8_t), then we are likely to invoke undefined
// behavior. Instead, the comparison function should accept a much wider array
// of types and convert up to the common type.

// Equality
template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow
>
constexpr bool operator==(ranged_integer<lhs_min, lhs_max, lhs_overflow> const lhs, ranged_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	using common_t = typename common_type_t<typename std::decay<decltype(lhs)>::type, typename std::decay<decltype(rhs)>::type>::underlying_type const;
	return static_cast<common_t>(lhs) == static_cast<common_t>(rhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow
>
constexpr bool operator!=(ranged_integer<lhs_min, lhs_max, lhs_overflow> const lhs, ranged_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	return !(lhs == rhs);
}



// Equality with built-ins

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
constexpr bool operator==(ranged_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return lhs == make_ranged(rhs);
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
constexpr bool operator==(integer const lhs, ranged_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return make_ranged(lhs) == rhs;
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
constexpr bool operator!=(ranged_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return !(lhs == rhs);
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
constexpr bool operator!=(integer const lhs, ranged_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return !(lhs == rhs);
}



// Relational operators

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow
>
constexpr bool operator<(ranged_integer<lhs_min, lhs_max, lhs_overflow> const lhs, ranged_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	using common_t = typename common_type_t<typename std::decay<decltype(lhs)>::type, typename std::decay<decltype(rhs)>::type>::underlying_type const;
	return static_cast<common_t>(lhs) < static_cast<common_t>(rhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow
>
constexpr bool operator>(ranged_integer<lhs_min, lhs_max, lhs_overflow> const lhs, ranged_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	return rhs < lhs;
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow
>
constexpr bool operator<=(ranged_integer<lhs_min, lhs_max, lhs_overflow> const lhs, ranged_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	return !(rhs < lhs);
}

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow
>
constexpr bool operator>=(ranged_integer<lhs_min, lhs_max, lhs_overflow> const lhs, ranged_integer<rhs_min, rhs_max, rhs_overflow> const rhs) noexcept {
	return !(lhs < rhs);
}



// Relational operators with built-ins

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
constexpr bool operator<(ranged_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return lhs < make_ranged(rhs);
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
constexpr bool operator<(integer const lhs, ranged_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return make_ranged(lhs) < rhs;
}

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
constexpr bool operator>(ranged_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return lhs > make_ranged(rhs);
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
constexpr bool operator>(integer const lhs, ranged_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return make_ranged(lhs) > rhs;
}

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
constexpr bool operator<=(ranged_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return lhs <= make_ranged(rhs);
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
constexpr bool operator<=(integer const lhs, ranged_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return make_ranged(lhs) <= rhs;
}

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
constexpr bool operator>=(ranged_integer<minimum, maximum, overflow_policy> const lhs, integer const rhs) noexcept {
	return lhs >= make_ranged(rhs);
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
constexpr bool operator>=(integer const lhs, ranged_integer<minimum, maximum, overflow_policy> const rhs) noexcept {
	return make_ranged(lhs) >= rhs;
}


#endif	// RANGED_INTEGER_COMPARISON_OPERATORS_HPP_
