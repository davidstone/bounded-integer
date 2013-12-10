// Compile-time min and max of an arbitrary number of values
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

#ifndef BOUNDED_INTEGER_MINMAX_HPP_
#define BOUNDED_INTEGER_MINMAX_HPP_

#include "common_type.hpp"
#include "ternary_conditional.hpp"

namespace bounded_integer {
namespace detail {

template<typename Function, typename Integer>
constexpr Integer minmax(Function &&, Integer && integer) {
	return integer;
}
template<typename Function, typename Integer, typename... Integers>
constexpr typename std::common_type<Integer, Integers...>::type minmax(Function && function, Integer && integer, Integers && ... integers) {
	// Only possibly move from the function object for the first call, interior
	// nested calls do not get passed an rvalue reference because we do not call
	// std::forward on the inner function.
	return std::forward<Function>(function)(std::forward<Integer>(integer), minmax(function, std::forward<Integers>(integers)...));
}

}	// namespace detail

template<typename... Integers>
constexpr typename std::common_type<Integers...>::type min(Integers && ... integers) {
	return detail::minmax(detail::less{}, std::forward<Integers>(integers)...);
}

template<typename... Integers>
constexpr typename std::common_type<Integers...>::type max(Integers && ... integers) {
	return detail::minmax(detail::greater{}, std::forward<Integers>(integers)...);
}

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_MINMAX_HPP_
