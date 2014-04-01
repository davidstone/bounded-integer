// Verify that the header can stand on its own
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

#include "class.hpp"
#include <type_traits>

namespace {

constexpr auto min = std::numeric_limits<int>::min();
constexpr auto max = std::numeric_limits<int>::max();
using type = bounded_integer::integer<min, max>;

static_assert(
	bounded_integer::detail::type_overlaps_range<std::decay_t<type>>(min, max),
	"Bounds of type do not overlap its own range."
);

static_assert(
	bounded_integer::detail::is_explicitly_constructible_from<bounded_integer::null_policy, type>(min, max),
	"Type is not explicitly constructible from itself."
);

static_assert(
	std::is_convertible<int, type>::value,
	"Cannot convert integer type to bounded_integer with same range."
);
static_assert(
	std::is_constructible<type, type, bounded_integer::non_check_t>::value,
	"Cannot construct a type from itself with non_check_t."
);

}	// namespace
