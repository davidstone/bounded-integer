// Determine if a class is a type of ranged_integer
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

#ifndef RANGED_INTEGER_IS_RANGED_INTEGER_HPP_
#define RANGED_INTEGER_IS_RANGED_INTEGER_HPP_

#include "forward_declaration.hpp"
#include <cstdint>
#include <type_traits>

template<typename integer, typename... integers>
class is_ranged_integer {
public:
	static constexpr bool value = is_ranged_integer<integer>::value and is_ranged_integer<integers...>::value;
};
template<typename integer>
class is_ranged_integer<integer> {
public:
	static constexpr bool value = false;
};
template<intmax_t minimum, intmax_t maximum, typename overflow_policy>
class is_ranged_integer<ranged_integer<minimum, maximum, overflow_policy>> {
public:
	static constexpr bool value = true;
};

template<typename integer, typename... integers>
class all_are_ranged_or_builtin_integer {
public:
	static constexpr bool value = all_are_ranged_or_builtin_integer<integer>::value and all_are_ranged_or_builtin_integer<integers...>::value;
};
template<typename integer>
class all_are_ranged_or_builtin_integer<integer> {
public:
	static constexpr bool value = std::is_integral<integer>::value or is_ranged_integer<integer>::value;
};

template<typename integer, typename... integers>
class any_are_ranged_integer {
public:
	static constexpr bool value = any_are_ranged_integer<integer>::value or any_are_ranged_integer<integers...>::value;
};
template<typename integer>
class any_are_ranged_integer<integer> {
public:
	static constexpr bool value = is_ranged_integer<integer>::value;
};

#endif	// RANGED_INTEGER_IS_RANGED_INTEGER_HPP_
