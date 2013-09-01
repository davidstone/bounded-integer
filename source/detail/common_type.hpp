// Type that is capable of holding the value in any of a list of integer types
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

#ifndef RANGED_INTEGER_COMMON_TYPE_HPP_
#define RANGED_INTEGER_COMMON_TYPE_HPP_

#include "class.hpp"
#include <type_traits>

namespace std {

// I do not have to specialize the single-argument version, as it just returns
// the type passed in, which will always work.

template<
	template<intmax_t, intmax_t> class OverflowPolicy,
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max
>
class common_type<ranged_integer<lhs_min, lhs_max, OverflowPolicy>, ranged_integer<rhs_min, rhs_max, OverflowPolicy>> {
private:
	static constexpr auto minimum = (lhs_min < rhs_min) ? lhs_min : rhs_min;
	static constexpr auto maximum = (lhs_max > rhs_max) ? lhs_max : rhs_max;
public:
	using type = ranged_integer<minimum, maximum, OverflowPolicy>;
};

template<
	template<intmax_t, intmax_t> class OverflowPolicy,
	intmax_t lhs_min, intmax_t lhs_max,
	intmax_t rhs_min, intmax_t rhs_max,
	typename... ranged_integers
>
class common_type<ranged_integer<lhs_min, lhs_max, OverflowPolicy>, ranged_integer<rhs_min, rhs_max, OverflowPolicy>, ranged_integers...> {
public:
	using type = typename common_type<
		ranged_integer<lhs_min, lhs_max, OverflowPolicy>,
		typename common_type<
			ranged_integer<rhs_min, rhs_max, OverflowPolicy>,
			ranged_integers...
		>::type
	>::type;
};
}	// namespace std

template<typename... Ts>
using common_type_t = typename std::common_type<Ts...>::type;

#endif	// RANGED_INTEGER_COMMON_TYPE_HPP_
