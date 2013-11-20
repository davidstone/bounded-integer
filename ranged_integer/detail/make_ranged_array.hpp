// Make a std::array of ranged_integer with automatically deduced size and type
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

#ifndef RANGED_INTEGER_MAKE_RANGED_ARRAY_HPP_
#define RANGED_INTEGER_MAKE_RANGED_ARRAY_HPP_

#include "common_type.hpp"
#include "is_ranged_integer.hpp"
#include "make_ranged.hpp"
#include <array>
#include <utility>

template<
	typename... Integers,
	typename result_type = std::array<common_type_t<decltype(make_ranged(std::declval<Integers>()))...>, sizeof...(Integers)>
>
constexpr result_type make_ranged_array(Integers && ... integers) noexcept {
	static_assert(all_are_ranged_or_builtin_integer<Integers...>::value, "Can only be called with integer values");
	return result_type{{make_ranged(std::forward<Integers>(integers))...}};
}

template<
	intmax_t... integers,
	typename result_type = std::array<common_type_t<decltype(make_ranged<integers>())...>, sizeof...(integers)>
>
constexpr result_type make_ranged_array() noexcept {
	return result_type{{make_ranged<integers>()...}};
}

#endif	// RANGED_INTEGER_MAKE_RANGED_ARRAY_HPP_
