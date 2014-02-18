// Determine if a class is a type of bounded_integer
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

#ifndef BOUNDED_INTEGER_IS_BOUNDED_INTEGER_HPP_
#define BOUNDED_INTEGER_IS_BOUNDED_INTEGER_HPP_

#include "forward_declaration.hpp"
#include <cstdint>
#include <type_traits>

namespace bounded_integer {

template<typename integer, typename... integers>
class is_bounded_integer {
public:
	static constexpr bool value = is_bounded_integer<integer>::value and is_bounded_integer<integers...>::value;
};
template<typename integer>
class is_bounded_integer<integer> {
public:
	static constexpr bool value = false;
};
template<intmax_t minimum, intmax_t maximum, typename overflow_policy>
class is_bounded_integer<bounded_integer<minimum, maximum, overflow_policy>> {
public:
	static constexpr bool value = true;
};

template<typename integer, typename... integers>
class all_are_bounded_or_builtin_integer {
public:
	static constexpr bool value = all_are_bounded_or_builtin_integer<integer>::value and all_are_bounded_or_builtin_integer<integers...>::value;
};
template<typename integer>
class all_are_bounded_or_builtin_integer<integer> {
public:
	static constexpr bool value = std::is_integral<integer>::value or is_bounded_integer<integer>::value;
};

template<typename integer, typename... integers>
class any_are_bounded_integer {
public:
	static constexpr bool value = any_are_bounded_integer<integer>::value or any_are_bounded_integer<integers...>::value;
};
template<typename integer>
class any_are_bounded_integer<integer> {
public:
	static constexpr bool value = is_bounded_integer<integer>::value;
};

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_IS_BOUNDED_INTEGER_HPP_
