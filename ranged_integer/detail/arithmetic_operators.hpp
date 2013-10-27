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
	using common_t = typename common_type_t<result_t, typename std::decay<decltype(lhs)>::type, typename std::decay<decltype(rhs)>::type>::underlying_type;
	// It is safe to use the non_check constructor because we have already
	// determined that the result will fit in result_t. We have to cast to the
	// intermediate common_t in case result_t is narrower than one of the
	// arguments.
	return result_t(op(static_cast<common_t>(lhs), static_cast<common_t>(rhs)), non_check);
}

}	// namespace detail


#define RANGED_INTEGER_OPERATOR_OVERLOADS(symbol, operator_name) \
template< \
	intmax_t lhs_min, intmax_t lhs_max, template<intmax_t, intmax_t> class lhs_overflow_policy, \
	intmax_t rhs_min, intmax_t rhs_max, template<intmax_t, intmax_t> class rhs_overflow_policy \
> \
constexpr auto operator symbol( \
	ranged_integer<lhs_min, lhs_max, lhs_overflow_policy> const lhs, \
	ranged_integer<rhs_min, rhs_max, rhs_overflow_policy> const rhs \
) noexcept -> decltype(detail::apply_operator(lhs, rhs, operator_name{})) { \
	return detail::apply_operator(lhs, rhs, operator_name{}); \
} \
 \
/* Interoperability with built-ins */ \
template< \
	intmax_t lhs_min, intmax_t lhs_max, \
	typename integer, \
	template<intmax_t, intmax_t> class overflow_policy, \
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
	template<intmax_t, intmax_t> class overflow_policy, \
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

#undef RANGED_INTEGER_OPERATOR_OVERLOADS


// Unary minus

template<
	intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy,
	typename result_type = unary_minus_result<minimum, maximum, overflow_policy>
>
constexpr result_type operator-(ranged_integer<minimum, maximum, overflow_policy> const value) noexcept {
	using common_type = common_type_t<result_type, ranged_integer<minimum, maximum, overflow_policy>>;
	return result_type(-static_cast<typename common_type::underlying_type>(value), non_check);
}


// Unary plus

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
constexpr ranged_integer<minimum, maximum, OverflowPolicy> operator+(ranged_integer<minimum, maximum, OverflowPolicy> const & value) noexcept {
	return value;
}

#endif	// RANGED_INTEGER_ARITHMETIC_OPERATORS_HPP_
