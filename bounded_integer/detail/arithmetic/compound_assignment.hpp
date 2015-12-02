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

// It seems as though it would be possible to write a common implementation of
// operator+= that can be used for any type that defines operator+ and
// operator=. The problem is with type compatibility. With a bounded::integer,
// the type returned by operator+ is not the same as the type of the left-hand
// side, which is what is assumed with the typical approach of implementing
// operator+ in terms of operator+=. The reverse is also problematic, however.
// It is also possible for two types to be valid arguments to operator+= without
// being valid arguments to operator+. This happens with bounded::integer with
// two different overflow policies and no defined relation -- for instance
// clamp_policy and modulo_policy. There is no 'best' type for operator+ to
// return when confronted with conflicting policies. However, for operator+=, we
// already know the result type is the type of the left-hand side, so we do not
// care about the policy of the right-hand side.
//
// An alternative is to change the definition of operator+ to always return
// null_policy, which would allow for a generic operator+= definition

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

}	// namespace bounded
