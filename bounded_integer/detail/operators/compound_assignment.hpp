// Compound assignment operators
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

#ifndef BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATORS_HPP_
#define BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATORS_HPP_

#include "arithmetic.hpp"
#include "../enable_if.hpp"
#include "../forward_declaration.hpp"
#include "../is_bounded_integer.hpp"
#include "../make.hpp"
#include <type_traits>

namespace bounded {

// We call make with the target's overflow policy because we need this exact
// overflow policy. We do not want to rely on common_policy here.
#define BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(symbol) \
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, typename T> \
decltype(auto) operator symbol##=(integer<minimum, maximum, overflow_policy, storage> & lhs, T && rhs) { \
	lhs = static_cast<integer<minimum, maximum, overflow_policy, storage>>(lhs symbol make<overflow_policy>(std::forward<T>(rhs))); \
	return lhs; \
} \
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, typename T> \
decltype(auto) operator symbol##=(integer<minimum, maximum, overflow_policy, storage> volatile & lhs, T && rhs) { \
	lhs = static_cast<integer<minimum, maximum, overflow_policy, storage>>(lhs symbol make<overflow_policy>(std::forward<T>(rhs))); \
	return lhs; \
}

// bounded::integer being assigned to
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(+)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(-)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(*)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(/)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(%)

#undef BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET


#define BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(symbol) \
template<typename T, intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, enable_if_t<detail::basic_numeric_limits<T>::is_integer and not is_bounded_integer<T>::value> = clang_dummy> \
decltype(auto) operator symbol(T & lhs, integer<minimum, maximum, overflow_policy, storage> const rhs) noexcept { \
	return lhs symbol rhs.value(); \
} \
template<typename T, intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, enable_if_t<detail::basic_numeric_limits<T>::is_integer> = clang_dummy> \
decltype(auto) operator symbol(T volatile & lhs, integer<minimum, maximum, overflow_policy, storage> const rhs) noexcept { \
	return lhs symbol rhs.value(); \
}

// bounded::integer being assigned from
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(+=)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(-=)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(*=)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(/=)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(%=)

#undef BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE


// Increment / decrement

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
decltype(auto) operator++(integer<minimum, maximum, overflow_policy, storage> & value) {
	value += make<1>();
	return value;
}
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
decltype(auto) operator++(integer<minimum, maximum, overflow_policy, storage> volatile & value) {
	value += make<1>();
	return value;
}

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
auto operator++(integer<minimum, maximum, overflow_policy, storage> & value, int) {
	auto previous = value;
	++value;
	return previous;
}
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
auto operator++(integer<minimum, maximum, overflow_policy, storage> volatile & value, int) {
	auto previous = value;
	++value;
	return previous;
}


template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
decltype(auto) operator--(integer<minimum, maximum, overflow_policy, storage> & value) {
	value -= make<1>();
	return value;
}
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
decltype(auto) operator--(integer<minimum, maximum, overflow_policy, storage> volatile & value) {
	value -= make<1>();
	return value;
}

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
auto operator--(integer<minimum, maximum, overflow_policy, storage> & value, int) {
	auto previous = value;
	--value;
	return previous;
}
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
auto operator--(integer<minimum, maximum, overflow_policy, storage> volatile & value, int) {
	auto previous = value;
	--value;
	return previous;
}

}	// namespace bounded
#endif	// BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATORS_HPP_
