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

#ifndef BOUNDED_INTEGER_OPERATORS_ARITHMETIC_BUILTIN_HPP_
#define BOUNDED_INTEGER_OPERATORS_ARITHMETIC_BUILTIN_HPP_

#include "arithmetic.hpp"
#include "../enable_if.hpp"
#include "../forward_declaration.hpp"
#include "../make.hpp"
#include "../numeric_limits.hpp"

#include <cstdint>

namespace bounded {

#define BOUNDED_INTEGER_OPERATOR_OVERLOADS(symbol, operator_name) \
template< \
	intmax_t lhs_min, intmax_t lhs_max, typename overflow, storage_type storage, typename T, \
	enable_if_t<basic_numeric_limits<T>::is_integer> = clang_dummy \
> \
constexpr auto operator symbol(integer<lhs_min, lhs_max, overflow, storage> const lhs, T const rhs) noexcept { \
	return lhs symbol make(rhs); \
} \
template< \
	typename T, intmax_t rhs_min, intmax_t rhs_max, typename overflow, storage_type storage, \
	enable_if_t<basic_numeric_limits<T>::is_integer> = clang_dummy \
> \
constexpr auto operator symbol(T const lhs, integer<rhs_min, rhs_max, overflow, storage> const rhs) noexcept { \
	return make(lhs) symbol rhs; \
}

BOUNDED_INTEGER_OPERATOR_OVERLOADS(+, detail::plus)
BOUNDED_INTEGER_OPERATOR_OVERLOADS(-, detail::minus)
BOUNDED_INTEGER_OPERATOR_OVERLOADS(*, detail::multiplies)
BOUNDED_INTEGER_OPERATOR_OVERLOADS(/, detail::divides)
BOUNDED_INTEGER_OPERATOR_OVERLOADS(%, detail::modulus)

BOUNDED_INTEGER_OPERATOR_OVERLOADS(<<, detail::left_shift)
BOUNDED_INTEGER_OPERATOR_OVERLOADS(>>, detail::right_shift)

#undef BOUNDED_INTEGER_OPERATOR_OVERLOADS


// Pointer overloads

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, typename T>
constexpr auto operator+(integer<minimum, maximum, overflow_policy, storage> const & number, T * const pointer) noexcept -> T * {
	return number.value() + pointer;
}

template<typename T, intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
constexpr auto operator+(T * const pointer, integer<minimum, maximum, overflow_policy, storage> const & number) noexcept -> T * {
	return pointer + number.value();
}

template<typename T, intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
constexpr auto operator-(T * const pointer, integer<minimum, maximum, overflow_policy, storage> const & number) noexcept -> T * {
	return pointer - number.value();
}

template<typename T, intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
decltype(auto) operator+=(T * & pointer, integer<minimum, maximum, overflow_policy, storage> const & number) noexcept {
	return pointer += number.value();
}

template<typename T, intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
decltype(auto) operator-=(T * & pointer, integer<minimum, maximum, overflow_policy, storage> const & number) noexcept {
	return pointer -= number.value();
}

// Not possible to overload operator[]. See
// https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/CmBERU_sr8Y

}	// namespace bounded

#endif	// BOUNDED_INTEGER_OPERATORS_ARITHMETIC_BUILTIN_HPP_
