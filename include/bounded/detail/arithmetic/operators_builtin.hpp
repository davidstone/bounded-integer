// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/operators.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/max_builtin.hpp>

#include <type_traits>

namespace bounded {

// Need this inner namespace to help work around
// https://github.com/saarraz/clang-concepts-monorepo/issues/18
namespace operators {

using ::bounded::detail_builtin_integer;

#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_INTEGRAL_CONSTANT(symbol) \
template<typename T, T rhs> \
constexpr auto operator symbol(bounded_integer auto const lhs, std::integral_constant<T, rhs>) noexcept { \
	return lhs symbol constant<rhs>; \
} \
template<typename T, T lhs> \
constexpr auto operator symbol(std::integral_constant<T, lhs>, bounded_integer auto const rhs) noexcept { \
	return constant<lhs> symbol rhs; \
}


#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(symbol, minimum, maximum) \
constexpr auto operator symbol(bounded_integer auto const lhs, detail_builtin_integer auto const rhs) noexcept { \
	return lhs.value() symbol rhs; \
} \
constexpr auto operator symbol(detail_builtin_integer auto const lhs, bounded_integer auto const rhs) noexcept { \
	return lhs symbol rhs.value(); \
} \
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_INTEGRAL_CONSTANT(symbol)


// Without the special versions for division and modulus, you could not use a
// built-in unsigned type as the right-hand side. Without special versions of
// shift operators, you could not use any built-in type as the right-hand side.

#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_BASIC(symbol) \
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(symbol, basic_numeric_limits<T>::min(), basic_numeric_limits<T>::max())


#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_DIVISION(symbol) \
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(symbol, 1, static_cast<detail::max_signed_t>(basic_numeric_limits<T>::max()))


#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_SHIFT(symbol) \
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(symbol, 0, (sizeof(detail::max_unsigned_t) * CHAR_BIT))


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

} // namespace operators

using ::bounded::operators::operator+;
using ::bounded::operators::operator*;
using ::bounded::operators::operator&;
using ::bounded::operators::operator/;
using ::bounded::operators::operator%;
using ::bounded::operators::operator<<;
using ::bounded::operators::operator>>;

} // namespace bounded

