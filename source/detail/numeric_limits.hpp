// Specialization of numeric_limits
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

#ifndef RANGED_INTEGER_NUMERIC_LIMITS_HPP_
#define RANGED_INTEGER_NUMERIC_LIMITS_HPP_

#include "class.hpp"
#include <limits>

template<intmax_t minimum, intmax_t maximum, template<intmax_t, intmax_t> class OverflowPolicy>
class numeric_limits<ranged_integer<minimum, maximum, OverflowPolicy>> {
public:
	static constexpr bool is_specialized = false;
};

#endif	// RANGED_INTEGER_NUMERIC_LIMITS_HPP_
