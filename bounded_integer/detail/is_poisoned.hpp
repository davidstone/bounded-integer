// Copyright (C) 2015 David Stone
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

#include <type_traits>

namespace bounded {
namespace detail {

template<typename T>
struct is_poisoned_c : std::true_type {};

template<intmax_t minimum, intmax_t maximum, typename policy, storage_type storage, bool poisoned>
struct is_poisoned_c<integer<minimum, maximum, policy, storage, poisoned>> : std::integral_constant<bool, poisoned> {};

template<typename T>
constexpr auto is_poisoned = is_poisoned_c<std::decay_t<T>>::value;

}	// namespace detail
}	// namespace bounded
