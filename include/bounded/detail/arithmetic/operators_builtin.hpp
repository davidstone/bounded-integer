// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/operators.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/is_bounded_integer.hpp>

#include <type_traits>

namespace bounded {

#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(symbol) \
constexpr auto operator symbol(bounded_integer auto const lhs, detail_builtin_integer auto const rhs) { \
	return lhs.value() symbol rhs; \
} \
constexpr auto operator symbol(detail_builtin_integer auto const lhs, bounded_integer auto const rhs) { \
	return lhs symbol rhs.value(); \
} \
\
template<typename T, T rhs> \
constexpr auto operator symbol(bounded_integer auto const lhs, std::integral_constant<T, rhs>) { \
	return lhs symbol constant<rhs>; \
} \
template<typename T, T lhs> \
constexpr auto operator symbol(std::integral_constant<T, lhs>, bounded_integer auto const rhs) { \
	return constant<lhs> symbol rhs; \
}


BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(+)
// binary minus uses the generic operator
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(*)
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(&)
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(/)
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(%)
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(<<)
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(>>)

#undef BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS

} // namespace bounded
