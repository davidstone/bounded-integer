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
#include "make_ranged.hpp"
#include "minmax.hpp"

#include <cstdint>
#include <type_traits>

namespace detail {
template<typename T>
class is_ranged_integer_c {
public:
	static constexpr bool value = false;
};
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy>
class is_ranged_integer_c<ranged_integer<minimum, maximum, overflow_policy>> {
public:
	static constexpr bool value = true;
};

template<typename T>
constexpr bool is_ranged_integer() noexcept {
	return is_ranged_integer_c<typename std::decay<T>::type>::value;
}

template<
	template<intmax_t, intmax_t> class overflow_policy,
	typename LHS,
	typename RHS,
	typename Operator,
	enable_if_t<detail::is_ranged_integer<LHS>() and detail::is_ranged_integer<RHS>()>...
>
constexpr operator_result<overflow_policy, LHS, RHS, Operator> apply_operator(LHS const & lhs, RHS const & rhs, Operator const & op) {
	using result_t = operator_result<overflow_policy, LHS, RHS, Operator>;
	using common_t = common_type_t<result_t, LHS, RHS>;
	// It is safe to use the non_check constructor because we have already
	// determined that the result will fit in result_t. We have to cast to the
	// intermediate common_t in case result_t is narrower than one of the
	// arguments.
	return result_t(op(static_cast<common_t>(lhs).value(), static_cast<common_t>(rhs).value()), non_check);
}

}	// namespace detail

// Addition
template<template<intmax_t, intmax_t> class overflow_policy, typename LHS, typename RHS>
constexpr auto add(LHS && lhs, RHS && rhs) noexcept -> decltype(detail::apply_operator<overflow_policy>(std::forward<LHS>(lhs), std::forward<RHS>(rhs), detail::plus{})) {
	return detail::apply_operator<overflow_policy>(std::forward<LHS>(lhs), std::forward<RHS>(rhs), detail::plus{});
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
) noexcept -> decltype(add<overflow_policy>(lhs, make_ranged(rhs))) {
	return add<overflow_policy>(lhs, make_ranged(rhs));
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
) noexcept -> decltype(add<overflow_policy>(make_ranged(lhs), rhs)) {
	return add<overflow_policy>(make_ranged(lhs), rhs);
}



// Subtraction

template<template<intmax_t, intmax_t> class overflow_policy, typename LHS, typename RHS>
constexpr auto subtract(LHS && lhs, RHS && rhs) noexcept -> decltype(detail::apply_operator<overflow_policy>(std::forward<LHS>(lhs), std::forward<RHS>(rhs), detail::minus{})) {
	return detail::apply_operator<overflow_policy>(std::forward<LHS>(lhs), std::forward<RHS>(rhs), detail::minus{});
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
) noexcept -> decltype(subtract<overflow_policy>(lhs, make_ranged(rhs))) {
	return subtract<overflow_policy>(lhs, make_ranged(rhs));
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
) noexcept -> decltype(subtract<overflow_policy>(make_ranged(lhs), rhs)) {
	return subtract<overflow_policy>(make_ranged(lhs), rhs);
}



// Multiplication
template<template<intmax_t, intmax_t> class overflow_policy, typename LHS, typename RHS>
constexpr auto multiply(LHS && lhs, RHS && rhs) noexcept -> decltype(detail::apply_operator<overflow_policy>(std::forward<LHS>(lhs), std::forward<RHS>(rhs), detail::multiplies{})) {
	return detail::apply_operator<overflow_policy>(std::forward<LHS>(lhs), std::forward<RHS>(rhs), detail::multiplies{});
}

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy
>
constexpr auto operator*(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(multiply<overflow_policy>(lhs, rhs)) {
	return multiply<overflow_policy>(lhs, rhs);
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
) noexcept -> decltype(multiply<overflow_policy>(lhs, make_ranged(rhs))) {
	return multiply<overflow_policy>(lhs, make_ranged(rhs));
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
) noexcept -> decltype(multiply<overflow_policy>(make_ranged(lhs), rhs)) {
	return multiply<overflow_policy>(make_ranged(lhs), rhs);
}



// Division
template<template<intmax_t, intmax_t> class overflow_policy, typename LHS, typename RHS>
constexpr auto divide(LHS && lhs, RHS && rhs) -> decltype(detail::apply_operator<overflow_policy>(std::forward<LHS>(lhs), std::forward<RHS>(rhs), detail::divides{})) {
	return detail::apply_operator<overflow_policy>(std::forward<LHS>(lhs), std::forward<RHS>(rhs), detail::divides{});
}

template<
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max,
	template<intmax_t, intmax_t> class overflow_policy
>
constexpr auto operator/(
	ranged_integer<lhs_min, lhs_max, overflow_policy> const lhs,
	ranged_integer<rhs_min, rhs_max, overflow_policy> const rhs
) noexcept -> decltype(divide<overflow_policy>(lhs, rhs)) {
	return divide<overflow_policy>(lhs, rhs);
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
) noexcept -> decltype(divide<overflow_policy>(lhs, make_ranged(rhs))) {
	return divide<overflow_policy>(lhs, make_ranged(rhs));
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
) noexcept -> decltype(divide<overflow_policy>(make_ranged(lhs), rhs)) {
	return divide<overflow_policy>(make_ranged(lhs), rhs);
}



// Unary minus

template<template<intmax_t, intmax_t> class overflow_policy, typename integer, enable_if_t<detail::is_ranged_integer<integer>()>...>
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
