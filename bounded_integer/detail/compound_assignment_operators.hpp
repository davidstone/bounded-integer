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

#include "arithmetic_operators.hpp"
#include "enable_if.hpp"
#include "forward_declaration.hpp"
#include <type_traits>

namespace bounded_integer {

// We call make_bounded with the target's overflow policy because we need this
// exact overflow policy. We do not want to rely on common_policy here.
#define BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(symbol) \
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound, typename integer> \
bounded_integer<minimum, maximum, overflow_policy, bound> & operator symbol##=(bounded_integer<minimum, maximum, overflow_policy, bound> & lhs, integer && rhs) { \
	lhs = static_cast<bounded_integer<minimum, maximum, overflow_policy, bound>>(lhs symbol make_bounded<overflow_policy>(std::forward<integer>(rhs))); \
	return lhs; \
} \
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound, typename integer> \
bounded_integer<minimum, maximum, overflow_policy, bound> volatile & operator symbol##=(bounded_integer<minimum, maximum, overflow_policy, bound> volatile & lhs, integer && rhs) { \
	lhs = static_cast<bounded_integer<minimum, maximum, overflow_policy, bound>>(lhs symbol make_bounded<overflow_policy>(std::forward<integer>(rhs))); \
	return lhs; \
}

// bounded_integer being assigned to
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(+)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(-)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(*)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(/)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(%)

#undef BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET


#define BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(symbol) \
template<typename integer, intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound, enable_if_t<std::is_integral<integer>::value> = clang_dummy> \
integer & operator symbol(integer & lhs, bounded_integer<minimum, maximum, overflow_policy, bound> const rhs) noexcept { \
	return lhs symbol rhs.value(); \
} \
template<typename integer, intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound, enable_if_t<std::is_integral<integer>::value> = clang_dummy> \
integer volatile & operator symbol(integer volatile & lhs, bounded_integer<minimum, maximum, overflow_policy, bound> const rhs) noexcept { \
	return lhs symbol rhs.value(); \
}

// bounded_integer being assigned from
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(+=)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(-=)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(*=)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(/=)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(%=)

#undef BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE


// Increment / decrement

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound>
bounded_integer<minimum, maximum, overflow_policy, bound> & operator++(bounded_integer<minimum, maximum, overflow_policy, bound> & value) {
	value += make_bounded<1>();
	return value;
}
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound>
bounded_integer<minimum, maximum, overflow_policy, bound> volatile & operator++(bounded_integer<minimum, maximum, overflow_policy, bound> volatile & value) {
	value += make_bounded<1>();
	return value;
}

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound>
bounded_integer<minimum, maximum, overflow_policy, bound> operator++(bounded_integer<minimum, maximum, overflow_policy, bound> & value, int) {
	auto const previous = value;
	++value;
	return previous;
}
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound>
bounded_integer<minimum, maximum, overflow_policy, bound> operator++(bounded_integer<minimum, maximum, overflow_policy, bound> volatile & value, int) {
	auto const previous = value;
	++value;
	return previous;
}


template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound>
bounded_integer<minimum, maximum, overflow_policy, bound> & operator--(bounded_integer<minimum, maximum, overflow_policy, bound> & value) {
	value -= make_bounded<1>();
	return value;
}
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound>
bounded_integer<minimum, maximum, overflow_policy, bound> volatile & operator--(bounded_integer<minimum, maximum, overflow_policy, bound> volatile & value) {
	value -= make_bounded<1>();
	return value;
}

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound>
bounded_integer<minimum, maximum, overflow_policy, bound> operator--(bounded_integer<minimum, maximum, overflow_policy, bound> & value, int) {
	auto const previous = value;
	--value;
	return previous;
}
template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound>
bounded_integer<minimum, maximum, overflow_policy, bound> operator--(bounded_integer<minimum, maximum, overflow_policy, bound> volatile & value, int) {
	auto const previous = value;
	--value;
	return previous;
}

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATORS_HPP_
