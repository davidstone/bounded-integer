// bounded_integer type
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

#ifndef BOUNDED_INTEGER_HPP_
#define BOUNDED_INTEGER_HPP_

#include "detail/class.hpp"
#include "detail/common_type.hpp"
#include "detail/conditional.hpp"
#include "detail/literal.hpp"
#include "detail/make_bounded.hpp"
#include "detail/math.hpp"
#include "detail/minmax.hpp"
#include "detail/numeric_limits.hpp"
#include "detail/operators.hpp"
#include "detail/policy/all.hpp"
#include "detail/stream.hpp"

// Missing from this list are array and optional classes

#include <cstdint>

namespace bounded_integer {

template<intmax_t minimum, intmax_t maximum, bounds bound = bounds::static_min_max>
using checked_integer = bounded_integer<minimum, maximum, throw_policy, bound>;

template<intmax_t minimum, intmax_t maximum, bounds bound = bounds::static_min_max>
using native_integer = bounded_integer<minimum, maximum, null_policy, bound>;

template<intmax_t minimum, intmax_t maximum, bounds bound = bounds::static_min_max>
using clamped_integer = bounded_integer<minimum, maximum, clamp_policy, bound>;

}	// namespace bounded_integer

#endif	// BOUNDED_INTEGER_HPP_
