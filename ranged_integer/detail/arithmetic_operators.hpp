// Addition, subtraction, multiplication, division, unary plus and minus, shifts
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

#ifndef RANGED_INTEGER_ARITHMETIC_OPERATORS_HPP_
#define RANGED_INTEGER_ARITHMETIC_OPERATORS_HPP_

#include "arithmetic_result_type.hpp"
#include "common_type.hpp"
#include "enable_if.hpp"
#include "forward_declaration.hpp"
#include "make_ranged.hpp"

#include <cstdint>
#include <type_traits>

// It is safe to use the non_check constructor because we already know that the
// result will fit in result_t. We have to cast to the intermediate common_t in
// case result_t is narrower than one of the arguments.
#define RANGED_INTEGER_OPERATOR_OVERLOADS(symbol, operator_name) \
template< \
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_overflow_policy, \
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_overflow_policy, \
	typename result_t = operator_result<lhs_min, lhs_max, lhs_overflow_policy, rhs_min, rhs_max, rhs_overflow_policy, operator_name> \
> \
constexpr result_t operator symbol( \
	ranged_integer<lhs_min, lhs_max, lhs_overflow_policy> const lhs, \
	ranged_integer<rhs_min, rhs_max, rhs_overflow_policy> const rhs \
) noexcept { \
	using common_t = typename common_type_t<result_t, decltype(lhs), decltype(rhs)>::underlying_type; \
	return result_t(operator_name{}(static_cast<common_t>(lhs), static_cast<common_t>(rhs)), non_check); \
} \
 \
/* Interoperability with built-ins */ \
template< \
	intmax_t lhs_min, intmax_t lhs_max, \
	typename integer, \
	typename overflow_policy, \
	enable_if_t<std::is_integral<integer>::value> = clang_dummy \
> \
constexpr auto operator symbol( \
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs, \
	integer const rhs \
) noexcept -> decltype(lhs symbol make_ranged(rhs)) { \
	return lhs symbol make_ranged(rhs); \
} \
template< \
	typename integer, \
	intmax_t rhs_min, intmax_t rhs_max, \
	typename overflow_policy, \
	enable_if_t<std::is_integral<integer>::value> = clang_dummy \
> \
constexpr auto operator symbol( \
	integer const lhs, \
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs \
) noexcept -> decltype(make_ranged(lhs) symbol rhs) { \
	return make_ranged(lhs) symbol rhs; \
}

RANGED_INTEGER_OPERATOR_OVERLOADS(+, detail::plus)
RANGED_INTEGER_OPERATOR_OVERLOADS(-, detail::minus)
RANGED_INTEGER_OPERATOR_OVERLOADS(*, detail::multiplies)
RANGED_INTEGER_OPERATOR_OVERLOADS(/, detail::divides)
RANGED_INTEGER_OPERATOR_OVERLOADS(%, detail::modulus)

RANGED_INTEGER_OPERATOR_OVERLOADS(<<, detail::left_shift)
RANGED_INTEGER_OPERATOR_OVERLOADS(>>, detail::right_shift)

#undef RANGED_INTEGER_OPERATOR_OVERLOADS


// Unary minus

template<
	intmax_t minimum, intmax_t maximum, typename overflow_policy,
	typename result_type = unary_minus_result<minimum, maximum, overflow_policy>
>
constexpr result_type operator-(ranged_integer<minimum, maximum, overflow_policy> const value) noexcept {
	using common_type = common_type_t<result_type, ranged_integer<minimum, maximum, overflow_policy>>;
	return result_type(-static_cast<typename common_type::underlying_type>(value), non_check);
}


// Unary plus

template<intmax_t minimum, intmax_t maximum, typename overflow_policy>
constexpr ranged_integer<minimum, maximum, overflow_policy> operator+(ranged_integer<minimum, maximum, overflow_policy> const value) noexcept {
	return value;
}

#endif	// RANGED_INTEGER_ARITHMETIC_OPERATORS_HPP_
