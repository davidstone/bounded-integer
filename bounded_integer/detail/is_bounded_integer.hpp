// Determine if a class is a type of bounded::integer
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

#pragma once

#include "forward_declaration.hpp"
#include <cstdint>
#include <type_traits>

namespace bounded {

template<typename T>
struct is_bounded_integer : std::false_type {};

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage>
struct is_bounded_integer<integer<minimum, maximum, overflow_policy, storage>> : std::true_type {};

}	// namespace bounded
