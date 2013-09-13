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
#include "forward_declaration.hpp"

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class overflow_policy, typename integer>
ranged_integer<minimum, maximum, overflow_policy> & operator+=(ranged_integer<minimum, maximum, overflow_policy> & ranged, integer const & other) {
	ranged = static_cast<ranged_integer<minimum, maximum, overflow_policy>>(ranged + make_ranged<overflow_policy>(other));
	return ranged;
}

#endif	// RANGED_INTEGER_COMPOUND_ASSIGNMENT_OPERATORS_HPP_
