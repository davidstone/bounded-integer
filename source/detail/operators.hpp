// Ranged integer operators
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

#ifndef RANGED_INTEGER_OPERATORS_HPP_
#define RANGED_INTEGER_OPERATORS_HPP_

#include "class.hpp"

#include <cstdint>

template<
	template<intmax_t, intmax_t> class result_overflow_policy,
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow_policy,
	typename result_type = ranged_integer<lhs_min + rhs_min, lhs_max + rhs_max, result_overflow_policy>
>
constexpr result_type add(
	ranged_integer<lhs_min, lhs_max, lhs_overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, rhs_overflow_policy> const rhs
) noexcept {
	return result_type(lhs.value() + rhs.value(), non_check);
}

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy
>
constexpr auto operator+(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(add<overflow_policy>(lhs, rhs)) {
	return add<overflow_policy>(lhs, rhs);
}

template<
	template<intmax_t, intmax_t> class result_overflow_policy,
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow_policy,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow_policy,
	typename result_type = ranged_integer<lhs_min - rhs_max, lhs_max - rhs_min, result_overflow_policy>
>
constexpr result_type subtract(
	ranged_integer<lhs_min, lhs_max, lhs_overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, rhs_overflow_policy> const rhs
) noexcept {
	return result_type(lhs.value() + rhs.value(), non_check);
}

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy
>
constexpr auto operator-(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(subtract<overflow_policy>(lhs, rhs)) {
	return subtract<overflow_policy>(lhs, rhs);
}

#endif	// RANGED_INTEGER_OPERATORS_HPP_
