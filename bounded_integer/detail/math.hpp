// Overloads for the standard library math functions.
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

#ifndef BOUNDED_INTEGER_MATH_HPP_
#define BOUNDED_INTEGER_MATH_HPP_

#include "arithmetic_operators.hpp"
#include "common_type.hpp"
#include "forward_declaration.hpp"
#include "make_bounded.hpp"
#include "minmax.hpp"
#include <type_traits>

namespace bounded_integer {

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounds bound>
constexpr auto abs(bounded_integer<minimum, maximum, overflow_policy, bound> const value) noexcept {
	// The 0 has to be there to restrict the range of possible values. Without
	// it, abs(bounded_integer<-7, 3>) would be [-3, 7] instead of [0, 7].
	return max(value, -value, make_bounded<0, overflow_policy, bound>());
}

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_MATH_HPP_
