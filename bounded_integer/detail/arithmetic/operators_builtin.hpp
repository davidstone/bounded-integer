// Addition, subtraction, multiplication, division, unary plus and minus, shifts
// Copyright (C) 2015 David Stone
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

#pragma once

#include "operators.hpp"
#include "../basic_numeric_limits.hpp"
#include "../forward_declaration.hpp"
#include "../make.hpp"
#include "../requires.hpp"

#include <cstdint>

namespace bounded {

// Without the special versions for division and modulus, you could not use a
// built-in unsigned type as the right-hand side. Without special versions of
// shift operators, you could not use any built-in type as the right-hand side.

#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(symbol, minimum, maximum) \
template< \
	intmax_t lhs_min, intmax_t lhs_max, typename overflow, storage_type storage, bool poisoned, typename T, \
	BOUNDED_REQUIRES(basic_numeric_limits<T>::is_integer) \
> \
constexpr auto operator symbol(integer<lhs_min, lhs_max, overflow, storage, poisoned> const lhs, T const rhs) noexcept { \
	return lhs symbol integer<minimum, maximum, overflow, storage, true>(rhs); \
} \
template< \
	typename T, intmax_t rhs_min, intmax_t rhs_max, typename overflow, storage_type storage, bool poisoned, \
	BOUNDED_REQUIRES(basic_numeric_limits<T>::is_integer) \
> \
constexpr auto operator symbol(T const lhs, integer<rhs_min, rhs_max, overflow, storage, poisoned> const rhs) noexcept { \
	return make(lhs) symbol rhs; \
}


#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_BASIC(symbol) \
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(symbol, static_cast<std::intmax_t>(basic_numeric_limits<T>::min()), static_cast<std::intmax_t>(basic_numeric_limits<T>::max()))


#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_DIVISION(symbol) \
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(symbol, 1, static_cast<std::intmax_t>(basic_numeric_limits<T>::max()))


#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_SHIFT(symbol) \
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(symbol, 0, std::numeric_limits<std::intmax_t>::digits)


BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_BASIC(+)
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS_BASIC(-)
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

}	// namespace bounded

