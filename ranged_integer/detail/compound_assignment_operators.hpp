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

namespace detail {

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer, typename Operator>
ranged_integer<minimum, maximum, overflow_policy> & compound_assignment(ranged_integer<minimum, maximum, overflow_policy> & lhs, integer const & rhs, Operator const & op) {
	// I explicitly call make_ranged with the target's overflow policy because
	// we need the result to be this exact overflow policy. We do not want to
	// rely on common_policy here.
	lhs = static_cast<ranged_integer<minimum, maximum, overflow_policy>>(op(lhs, make_ranged<overflow_policy>(rhs)));
	return lhs;
}

}	// namespace detail

// ranged_integer being assigned to

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer>
ranged_integer<minimum, maximum, overflow_policy> & operator+=(ranged_integer<minimum, maximum, overflow_policy> & lhs, integer const & rhs) {
	return compound_assignment(lhs, rhs, detail::plus{});
}

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer>
ranged_integer<minimum, maximum, overflow_policy> & operator-=(ranged_integer<minimum, maximum, overflow_policy> & lhs, integer const & rhs) {
	return compound_assignment(lhs, rhs, detail::minus{});
}

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer>
ranged_integer<minimum, maximum, overflow_policy> & operator*=(ranged_integer<minimum, maximum, overflow_policy> & lhs, integer const & rhs) {
	return compound_assignment(lhs, rhs, detail::multiplies{});
}

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer>
ranged_integer<minimum, maximum, overflow_policy> & operator/=(ranged_integer<minimum, maximum, overflow_policy> & lhs, integer const & rhs) {
	return compound_assignment(lhs, rhs, detail::divides{});
}

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer>
ranged_integer<minimum, maximum, overflow_policy> & operator%=(ranged_integer<minimum, maximum, overflow_policy> & lhs, integer const & rhs) {
	return compound_assignment(lhs, rhs, detail::modulus{});
}


// ranged_integer being assigned from

template<typename integer, intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
integer & operator+=(integer & lhs, ranged_integer<minimum, maximum, overflow_policy> const & rhs) noexcept {
	return lhs += rhs.value();
}

template<typename integer, intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
integer & operator-=(integer & lhs, ranged_integer<minimum, maximum, overflow_policy> const & rhs) noexcept {
	return lhs -= rhs.value();
}

template<typename integer, intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
integer & operator*=(integer & lhs, ranged_integer<minimum, maximum, overflow_policy> const & rhs) noexcept {
	return lhs *= rhs.value();
}

template<typename integer, intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
integer & operator/=(integer & lhs, ranged_integer<minimum, maximum, overflow_policy> const & rhs) noexcept {
	return lhs /= rhs.value();
}

template<typename integer, intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, enable_if_t<std::is_integral<integer>::value> = clang_dummy>
integer & operator%=(integer & lhs, ranged_integer<minimum, maximum, overflow_policy> const & rhs) noexcept {
	return lhs %= rhs.value();
}



// Increment / decrement

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
ranged_integer<minimum, maximum, OverflowPolicy> & operator++(ranged_integer<minimum, maximum, OverflowPolicy> & value) {
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
ranged_integer<minimum, maximum, OverflowPolicy> & operator--(ranged_integer<minimum, maximum, OverflowPolicy> & value) {
	value -= make_ranged<1>();
	return value;
}

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
ranged_integer<minimum, maximum, OverflowPolicy> operator--(ranged_integer<minimum, maximum, OverflowPolicy> & value, int) {
	auto const previous = value;
	--value;
	return previous;
}



#endif	// RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATORS_HPP_
