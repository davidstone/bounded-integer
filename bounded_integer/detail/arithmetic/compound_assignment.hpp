// Compound assignment operators
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
#include "../is_bounded_integer.hpp"
#include "../make.hpp"
#include "../noexcept.hpp"
#include "../requires.hpp"

#include <type_traits>

namespace bounded {

// We call make with the target's overflow policy because we need this exact
// overflow policy. We do not want to rely on common_policy here.
#define BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(symbol) \
template<typename LHS, typename RHS, BOUNDED_REQUIRES(is_bounded_integer<LHS> and basic_numeric_limits<RHS>::is_integer)> \
constexpr decltype(auto) operator symbol##=(LHS & lhs, RHS && rhs) BOUNDED_NOEXCEPT( \
	lhs = static_cast<std::decay_t<LHS>>(lhs symbol make<typename std::remove_reference_t<LHS>::overflow_policy>(std::forward<RHS>(rhs))) \
) \

// bounded::integer being assigned to
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(+)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(-)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(*)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(/)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET(%)

#undef BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_TARGET


#define BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(symbol) \
template<typename LHS, typename RHS, BOUNDED_REQUIRES(basic_numeric_limits<LHS>::is_integer and not is_bounded_integer<LHS> and is_bounded_integer<RHS>)> \
constexpr decltype(auto) operator symbol(LHS & lhs, RHS const & rhs) BOUNDED_NOEXCEPT( \
	lhs symbol rhs.value() \
)

// bounded::integer being assigned from
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(+=)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(-=)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(*=)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(/=)
BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE(%=)

#undef BOUNDED_INTEGER_COMPOUND_ASSIGNMENT_OPERATOR_SOURCE


// Increment / decrement

template<typename Integer, BOUNDED_REQUIRES(is_bounded_integer<Integer>)>
constexpr decltype(auto) operator++(Integer & value) BOUNDED_NOEXCEPT(
	value += constant<1>
)

template<typename Integer, BOUNDED_REQUIRES(is_bounded_integer<Integer>)>
constexpr auto operator++(Integer & value, int) noexcept(std::is_nothrow_copy_constructible<Integer>::value and noexcept(++value)) {
	auto previous = value;
	++value;
	return previous;
}


template<typename Integer, BOUNDED_REQUIRES(is_bounded_integer<Integer>)>
constexpr decltype(auto) operator--(Integer & value) BOUNDED_NOEXCEPT(
	value -= constant<1>
)

template<typename Integer, BOUNDED_REQUIRES(is_bounded_integer<Integer>)>
constexpr auto operator--(Integer & value, int) noexcept(std::is_nothrow_copy_constructible<Integer>::value and noexcept(--value)) {
	auto previous = value;
	--value;
	return previous;
}

}	// namespace bounded
