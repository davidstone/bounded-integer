// Overloads for the standard library math functions.
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

#ifndef RANGED_INTEGER_MATH_HPP_
#define RANGED_INTEGER_MATH_HPP_

#include "arithmetic_operators.hpp"
#include "common_type.hpp"
#include "forward_declaration.hpp"
#include "minmax.hpp"
#include <type_traits>

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy, typename result_type = ranged_integer<max(0, minimum, -maximum), max(maximum, -minimum), OverflowPolicy>>
constexpr result_type abs(ranged_integer<minimum, maximum, OverflowPolicy> const & value) noexcept {
	// We have to cast the value to the common type of the argument and result
	// type before we negate because -min might not be in the value, even if we
	// were to cast to the underlying type first.
	using common_t = common_type_t<typename std::decay<decltype(value)>::type, result_type>;
	return (value >= 0) ?
		result_type(static_cast<common_t>(value), non_check) :
		result_type(-static_cast<common_t>(value), non_check);
}

#endif	// RANGED_INTEGER_MATH_HPP_
