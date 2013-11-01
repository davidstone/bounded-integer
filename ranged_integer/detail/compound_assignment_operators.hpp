// Compound assignment operators
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

#ifndef RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATORS_HPP_
#define RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATORS_HPP_

#include "arithmetic_operators.hpp"
#include "enable_if.hpp"
#include "forward_declaration.hpp"
#include <type_traits>

// We call make_ranged with the target's overflow policy because we need this
// exact overflow policy. We do not want to rely on common_policy here.
#define RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(symbol) \
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer> \
ranged_integer<minimum, maximum, overflow_policy> & operator symbol##=(ranged_integer<minimum, maximum, overflow_policy> & lhs, integer && rhs) { \
	lhs = static_cast<ranged_integer<minimum, maximum, overflow_policy>>(lhs symbol make_ranged<overflow_policy>(std::forward<integer>(rhs))); \
	return lhs; \
} \
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer> \
ranged_integer<minimum, maximum, overflow_policy> volatile & operator symbol##=(ranged_integer<minimum, maximum, overflow_policy> volatile & lhs, integer && rhs) { \
	lhs = static_cast<ranged_integer<minimum, maximum, overflow_policy>>(lhs symbol make_ranged<overflow_policy>(std::forward<integer>(rhs))); \
	return lhs; \
}

// ranged_integer being assigned to
RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(+)
RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(-)
RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(*)
RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(/)
RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(%)

#undef RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET


#define RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(symbol) \
template<typename integer, intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, enable_if_t<std::is_integral<integer>::value> = clang_dummy> \
integer & operator symbol(integer & lhs, ranged_integer<minimum, maximum, overflow_policy> const rhs) noexcept { \
	return lhs symbol rhs.value(); \
} \
template<typename integer, intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, enable_if_t<std::is_integral<integer>::value> = clang_dummy> \
integer volatile & operator symbol(integer volatile & lhs, ranged_integer<minimum, maximum, overflow_policy> const rhs) noexcept { \
	return lhs symbol rhs.value(); \
}

// ranged_integer being assigned from
RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(+=)
RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(-=)
RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(*=)
RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(/=)
RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(%=)

#undef RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE


// Increment / decrement

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
ranged_integer<minimum, maximum, OverflowPolicy> & operator++(ranged_integer<minimum, maximum, OverflowPolicy> & value) {
	value += make_ranged<1>();
	return value;
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
ranged_integer<minimum, maximum, OverflowPolicy> volatile & operator++(ranged_integer<minimum, maximum, OverflowPolicy> volatile & value) {
	value += make_ranged<1>();
	return value;
}

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
ranged_integer<minimum, maximum, OverflowPolicy> operator++(ranged_integer<minimum, maximum, OverflowPolicy> & value, int) {
	auto const previous = value;
	++value;
	return previous;
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
ranged_integer<minimum, maximum, OverflowPolicy> operator++(ranged_integer<minimum, maximum, OverflowPolicy> volatile & value, int) {
	auto const previous = value;
	++value;
	return previous;
}


template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
ranged_integer<minimum, maximum, OverflowPolicy> & operator--(ranged_integer<minimum, maximum, OverflowPolicy> & value) {
	value -= make_ranged<1>();
	return value;
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
ranged_integer<minimum, maximum, OverflowPolicy> volatile & operator--(ranged_integer<minimum, maximum, OverflowPolicy> volatile & value) {
	value -= make_ranged<1>();
	return value;
}

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
ranged_integer<minimum, maximum, OverflowPolicy> operator--(ranged_integer<minimum, maximum, OverflowPolicy> & value, int) {
	auto const previous = value;
	--value;
	return previous;
}
template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
ranged_integer<minimum, maximum, OverflowPolicy> operator--(ranged_integer<minimum, maximum, OverflowPolicy> volatile & value, int) {
	auto const previous = value;
	--value;
	return previous;
}



#endif	// RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATORS_HPP_
