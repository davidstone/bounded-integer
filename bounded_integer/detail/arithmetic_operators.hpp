// Addition, subtraction, multiplication, division, unary plus and minus, shifts
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

#ifndef BOUNDED_INTEGER_ARITHMETIC_OPERATORS_HPP_
#define BOUNDED_INTEGER_ARITHMETIC_OPERATORS_HPP_

#include "arithmetic_result_type.hpp"
#include "common_type.hpp"
#include "enable_if.hpp"
#include "forward_declaration.hpp"
#include "make_bounded.hpp"

#include <cstdint>
#include <type_traits>

namespace bounded_integer {

// TODO: consider how dynamic bounds fit into this

// It is safe to use the non_check constructor because we already know that the
// result will fit in result_t. We have to cast to the intermediate common_t in
// case result_t is narrower than one of the arguments.
#define BOUNDED_INTEGER_OPERATOR_OVERLOADS(symbol, operator_name) \
template< \
	intmax_t lhs_min, intmax_t lhs_max, typename lhs_policy, bounds lhs_bound, \
	intmax_t rhs_min, intmax_t rhs_max, typename rhs_policy, bounds rhs_bound \
> \
constexpr auto operator symbol( \
	bounded_integer<lhs_min, lhs_max, lhs_policy, lhs_bound> const lhs, \
	bounded_integer<rhs_min, rhs_max, rhs_policy, rhs_bound> const rhs \
) noexcept { \
	using result_t = operator_result<lhs_min, lhs_max, lhs_policy, rhs_min, rhs_max, rhs_policy, operator_name>; \
	using common_t = typename common_type_t<result_t, decltype(lhs), decltype(rhs)>::underlying_type; \
	return result_t(operator_name{}(static_cast<common_t>(lhs), static_cast<common_t>(rhs)), non_check); \
} \
 \
/* Interoperability with built-ins */ \
template< \
	intmax_t lhs_min, intmax_t lhs_max, typename overflow, bounds bound, typename integer, \
	enable_if_t<std::is_integral<integer>::value> = clang_dummy \
> \
constexpr auto operator symbol(bounded_integer<lhs_min, lhs_max, overflow, bound> const lhs, integer const rhs) noexcept { \
	return lhs symbol make_bounded(rhs); \
} \
template< \
	typename integer, intmax_t rhs_min, intmax_t rhs_max, typename overflow, bounds bound, \
	enable_if_t<std::is_integral<integer>::value> = clang_dummy \
> \
constexpr auto operator symbol(integer const lhs, bounded_integer<rhs_min, rhs_max, overflow, bound> const rhs) noexcept { \
	return make_bounded(lhs) symbol rhs; \
}

BOUNDED_INTEGER_OPERATOR_OVERLOADS(+, detail::plus)
BOUNDED_INTEGER_OPERATOR_OVERLOADS(-, detail::minus)
BOUNDED_INTEGER_OPERATOR_OVERLOADS(*, detail::multiplies)
BOUNDED_INTEGER_OPERATOR_OVERLOADS(/, detail::divides)
BOUNDED_INTEGER_OPERATOR_OVERLOADS(%, detail::modulus)

BOUNDED_INTEGER_OPERATOR_OVERLOADS(<<, detail::left_shift)
BOUNDED_INTEGER_OPERATOR_OVERLOADS(>>, detail::right_shift)

#undef BOUNDED_INTEGER_OPERATOR_OVERLOADS


// Unary minus

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound>
constexpr auto operator-(bounded_integer<minimum, maximum, overflow_policy, bound> const value) noexcept {
	using result_type = unary_minus_result<minimum, maximum, overflow_policy>;
	using common_type = common_type_t<result_type, bounded_integer<minimum, maximum, overflow_policy, bound>>;
	return result_type(-static_cast<typename common_type::underlying_type>(value), non_check);
}


// Unary plus

// This is currently the only operator that preserves dynamic bounds...
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound>
constexpr auto operator+(bounded_integer<minimum, maximum, overflow_policy, bound> const value) noexcept {
	return value;
}

}	// namespace bounded_integer

#endif	// BOUNDED_INTEGER_ARITHMETIC_OPERATORS_HPP_
