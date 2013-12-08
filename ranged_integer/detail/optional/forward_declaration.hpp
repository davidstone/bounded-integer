// Forward declaration for optional
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

#ifndef RANGED_INTEGER_DETAIL_OPTIONAL_FORWARD_DECLARATION_HPP_
#define RANGED_INTEGER_DETAIL_OPTIONAL_FORWARD_DECLARATION_HPP_

#include <cstdint>

enum class none_t {};
constexpr none_t none = none_t{};

namespace detail {

template<intmax_t minimum, intmax_t maximum, typename policy, typename enable = void>
class optional;

}	// namespace detail

#endif	// RANGED_INTEGER_DETAIL_OPTIONAL_FORWARD_DECLARATION_HPP_
