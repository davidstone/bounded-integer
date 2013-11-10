// Ranged integer type
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

#ifndef RANGED_INTEGER_HPP_
#define RANGED_INTEGER_HPP_

#include "detail/class.hpp"
#include "detail/common_type.hpp"
#include "detail/literal.hpp"
#include "detail/make_ranged.hpp"
#include "detail/make_ranged_array.hpp"
#include "detail/minmax.hpp"
#include "detail/numeric_limits.hpp"
#include "detail/operators.hpp"
#include "detail/policy.hpp"
#include "detail/stream.hpp"

#include <cstdint>

template<intmax_t minimum, intmax_t maximum>
using checked_integer = ranged_integer<minimum, maximum, throw_on_overflow>;

template<intmax_t minimum, intmax_t maximum>
using native_integer = ranged_integer<minimum, maximum, null_policy>;

template<intmax_t minimum, intmax_t maximum>
using clamped_integer = ranged_integer<minimum, maximum, clamp_on_overflow>;

#endif	// RANGED_INTEGER_HPP_
