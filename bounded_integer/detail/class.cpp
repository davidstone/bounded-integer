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
#include "make_bounded.hpp"
#include <type_traits>

namespace {

using base_type = bounded_integer::detail::base<std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), bounded_integer::null_policy>;
using type = bounded_integer::equivalent_type<int>;

static_assert(std::is_convertible<int, base_type>::value, "Cannot convert integer type to bounded_integer base_type with same range.");
static_assert(std::is_convertible<int, type>::value, "Cannot convert integer type to bounded_integer with same range.");
static_assert(std::is_constructible<type, type, bounded_integer::non_check_t>::value, "Cannot construct a type from itself with non_check_t.");

}	// namespace
