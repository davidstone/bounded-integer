// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/operators.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/requires.hpp>

#include <cstdint>
#include <limits>
#include <type_traits>

namespace bounded {


#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_INTEGRAL_CONSTANT(symbol) \
template<typename LHS, typename T, T rhs, BOUNDED_REQUIRES(is_bounded_integer<LHS>)> \
constexpr auto operator symbol(LHS const lhs, std::integral_constant<T, rhs>) noexcept { \
	return lhs symbol constant<rhs>; \
} \
template<typename T, T lhs, typename RHS, BOUNDED_REQUIRES(is_bounded_integer<RHS>)> \
constexpr auto operator symbol(std::integral_constant<T, lhs>, RHS const rhs) noexcept { \
	return constant<lhs> symbol rhs; \
}


// Many standard library components do things like mutiply by some integer (such
// as 2, or sizeof(T)). This can quickly lead to growth in the bounds that
// exceeds intmax_t, even though the math is perfectly safe. By default, the
// result of mixed arithmetic between bounded::integer and built-in integers is
// a built-in integer. If this macro is defined, it is a bounded::integer with
// the poisoned bit set to true.

#ifndef BOUNDED_MIXED_ARITHMETIC_RESULT_IS_BUILTIN

#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(symbol, minimum, maximum) \
template< \
	intmax_t lhs_min, intmax_t lhs_max, typename overflow, storage_type storage, bool poisoned, typename T, \
	BOUNDED_REQUIRES(std::is_integral<T>{}) \
> \
constexpr auto operator symbol(integer<lhs_min, lhs_max, overflow, storage, poisoned> const lhs, T const rhs) noexcept { \
	return lhs symbol integer<minimum, maximum, overflow, storage, true>(rhs); \
} \
template< \
	typename T, intmax_t rhs_min, intmax_t rhs_max, typename overflow, storage_type storage, bool poisoned, \
	BOUNDED_REQUIRES(std::is_integral<T>{}) \
> \
constexpr auto operator symbol(T const lhs, integer<rhs_min, rhs_max, overflow, storage, poisoned> const rhs) noexcept { \
	return detail::basic_integer(lhs, overflow{}) symbol rhs; \
} \
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_INTEGRAL_CONSTANT(symbol)


#else

#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(symbol, minimum, maximum) \
template<typename LHS, typename RHS, BOUNDED_REQUIRES(is_bounded_integer<LHS> and std::is_integral<RHS>{})> \
constexpr auto operator symbol(LHS const lhs, RHS const rhs) noexcept { \
	return lhs.value() symbol rhs; \
} \
template<typename LHS, typename RHS, BOUNDED_REQUIRES(std::is_integral<LHS>{} and is_bounded_integer<RHS>)> \
constexpr auto operator symbol(LHS const lhs, RHS const rhs) noexcept { \
	return lhs symbol rhs.value(); \
} \
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_INTEGRAL_CONSTANT(symbol)


#endif


// Without the special versions for division and modulus, you could not use a
// built-in unsigned type as the right-hand side. Without special versions of
// shift operators, you could not use any built-in type as the right-hand side.

#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_BASIC(symbol) \
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(symbol, static_cast<std::intmax_t>(std::numeric_limits<T>::min()), static_cast<std::intmax_t>(std::numeric_limits<T>::max()))


#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_DIVISION(symbol) \
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(symbol, 1, static_cast<std::intmax_t>(std::numeric_limits<T>::max()))


#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_SHIFT(symbol) \
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(symbol, 0, std::numeric_limits<std::intmax_t>::digits)


BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_BASIC(+)
// binary minus uses the generic operator
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_BASIC(*)
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_BASIC(&)

BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_DIVISION(/)
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_DIVISION(%)

BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_SHIFT(<<)
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_SHIFT(>>)

#undef BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_SHIFT
#undef BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_DIVISION
#undef BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_BASIC
#undef BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS
#undef BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_INTEGRAL_CONSTANT

}	// namespace bounded

