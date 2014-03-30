// typedefs
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

#ifndef BOUNDED_INTEGER_TYPEDEFS_HPP_
#define BOUNDED_INTEGER_TYPEDEFS_HPP_

#include "forward_declaration.hpp"
#include "policy/all.hpp"

#include <cstdint>

namespace bounded_integer {

template<intmax_t minimum, intmax_t maximum>
using checked_integer = integer<minimum, maximum, throw_policy>;

template<intmax_t minimum, intmax_t maximum>
using clamped_integer = integer<minimum, maximum, clamp_policy>;

template<intmax_t minimum, intmax_t maximum, typename base_overflow_policy = throw_policy>
using dynamic_integer = integer<minimum, maximum, dynamic_policy<minimum, maximum, throw_policy>>;

template<intmax_t minimum, intmax_t maximum, typename base_overflow_policy = throw_policy>
using dynamic_min_integer = integer<minimum, maximum, dynamic_min_policy<minimum, maximum, throw_policy>>;

template<intmax_t minimum, intmax_t maximum, typename base_overflow_policy = throw_policy>
using dynamic_max_integer = integer<minimum, maximum, dynamic_max_policy<minimum, maximum, throw_policy>>;

}	// namespace bounded_integer

#endif	// BOUNDED_INTEGER_TYPEDEFS_HPP_
