// Addition, subtraction, multiplication, division, unary minus
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
#include "is_ranged_integer.hpp"
#include "make_ranged.hpp"
#include "minmax.hpp"

#include <cstdint>
#include <type_traits>

namespace detail {

template<
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow,
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow,
	typename Operator
>
constexpr operator_result<lhs_min, lhs_max, lhs_overflow, rhs_min, rhs_max, rhs_overflow, Operator> apply_operator(ranged_integer<lhs_min, lhs_max, lhs_overflow> const & lhs, ranged_integer<rhs_min, rhs_max, rhs_overflow> const & rhs, Operator const & op) {
	using result_t = operator_result<lhs_min, lhs_max, lhs_overflow, rhs_min, rhs_max, rhs_overflow, Operator>;
	using common_t = common_type_t<result_t, typename std::decay<decltype(lhs)>::type, typename std::decay<decltype(rhs)>::type>;
	// It is safe to use the non_check constructor because we have already
	// determined that the result will fit in result_t. We have to cast to the
	// intermediate common_t in case result_t is narrower than one of the
	// arguments.
	return result_t(op(static_cast<common_t>(lhs).value(), static_cast<common_t>(rhs).value()), non_check);
}

}	// namespace detail

// Addition
template<typename LHS, typename RHS>
constexpr auto add(LHS const & lhs, RHS const & rhs) noexcept -> decltype(detail::apply_operator(lhs, rhs, detail::plus{})) {
	return detail::apply_operator(lhs, rhs, detail::plus{});
}

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy
>
constexpr auto operator+(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(add(lhs, rhs)) {
	return add(lhs, rhs);
}

// Addition with built-ins
template<
	intmax_t lhs_min, intmax_t lhs_max,
	typename integer,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator+(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	integer const rhs
) noexcept -> decltype(add(lhs, make_ranged(rhs))) {
	return add(lhs, make_ranged(rhs));
}
template<
	typename integer,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator+(
	integer const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(add(make_ranged(lhs), rhs)) {
	return add(make_ranged(lhs), rhs);
}



// Subtraction

template<typename LHS, typename RHS>
constexpr auto subtract(LHS const & lhs, RHS const & rhs) noexcept -> decltype(detail::apply_operator(lhs, rhs, detail::minus{})) {
	return detail::apply_operator(lhs, rhs, detail::minus{});
}

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy
>
constexpr auto operator-(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(subtract(lhs, rhs)) {
	return subtract(lhs, rhs);
}

// Subtraction with built-ins
template<
	intmax_t lhs_min, intmax_t lhs_max,
	typename integer,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator-(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	integer const rhs
) noexcept -> decltype(subtract(lhs, make_ranged(rhs))) {
	return subtract(lhs, make_ranged(rhs));
}
template<
	typename integer,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator-(
	integer const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(subtract(make_ranged(lhs), rhs)) {
	return subtract(make_ranged(lhs), rhs);
}



// Multiplication
template<typename LHS, typename RHS>
constexpr auto multiply(LHS const & lhs, RHS const & rhs) noexcept -> decltype(detail::apply_operator(lhs, rhs, detail::multiplies{})) {
	return detail::apply_operator(lhs, rhs, detail::multiplies{});
}

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy
>
constexpr auto operator*(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(multiply(lhs, rhs)) {
	return multiply(lhs, rhs);
}

// Multiplication with built-ins
template<
	intmax_t lhs_min, intmax_t lhs_max,
	typename integer,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator*(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	integer const rhs
) noexcept -> decltype(multiply(lhs, make_ranged(rhs))) {
	return multiply(lhs, make_ranged(rhs));
}
template<
	typename integer,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator*(
	integer const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(multiply(make_ranged(lhs), rhs)) {
	return multiply(make_ranged(lhs), rhs);
}



// Division
template<typename LHS, typename RHS>
constexpr auto divide(LHS const & lhs, RHS const & rhs) noexcept -> decltype(detail::apply_operator(lhs, rhs, detail::divides{})) {
	return detail::apply_operator(lhs, rhs, detail::divides{});
}

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy
>
constexpr auto operator/(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(divide(lhs, rhs)) {
	return divide(lhs, rhs);
}

// Division with built-ins
template<
	intmax_t lhs_min, intmax_t lhs_max,
	typename integer,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator/(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	integer const rhs
) noexcept -> decltype(divide(lhs, make_ranged(rhs))) {
	return divide(lhs, make_ranged(rhs));
}
template<
	typename integer,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator/(
	integer const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(divide(make_ranged(lhs), rhs)) {
	return divide(make_ranged(lhs), rhs);
}



// Modulo
template<typename LHS, typename RHS>
constexpr auto modulo(LHS const & lhs, RHS const & rhs) noexcept -> decltype(detail::apply_operator(lhs, rhs, detail::modulus{})) {
	return detail::apply_operator(lhs, rhs, detail::modulus{});
}

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy
>
constexpr auto operator%(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(modulo(lhs, rhs)) {
	return modulo(lhs, rhs);
}

// Modulo with built-ins
template<
	intmax_t lhs_min, intmax_t lhs_max,
	typename integer,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator%(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	integer const rhs
) noexcept -> decltype(modulo(lhs, make_ranged(rhs))) {
	return modulo(lhs, make_ranged(rhs));
}
template<
	typename integer,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy,
	enable_if_t<std::is_integral<integer>::value>...
>
constexpr auto operator%(
	integer const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(modulo(make_ranged(lhs), rhs)) {
	return modulo(make_ranged(lhs), rhs);
}



// Unary minus

template<template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<is_ranged_integer<integer>()>...>
constexpr unary_minus_result<overflow_policy, integer> negate(integer value) noexcept {
	using result_type = unary_minus_result<overflow_policy, integer>;
	using common_type = common_type_t<result_type, integer>;
	return result_type(-static_cast<typename common_type::underlying_type>(value), non_check);
}

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy>
constexpr auto operator-(ranged_integer<minimum, maximum, overflow_policy> const value) noexcept -> decltype(negate<overflow_policy>(value)) {
	return negate<overflow_policy>(value);
}



// Unary plus

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
constexpr ranged_integer<minimum, maximum, OverflowPolicy> operator+(ranged_integer<minimum, maximum, OverflowPolicy> const & value) noexcept {
	return value;
}

#endif	// RANGED_INTEGER_ARITHMETIC_OPERATORS_HPP_
