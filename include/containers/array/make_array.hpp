// Make an array with automatically deduced size and type
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

#include <containers/array/array.hpp>

#include <bounded_integer/bounded_integer.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

template<std::size_t number_of_values, std::size_t... dimensions>
struct final_dimension {
	static_assert(number_of_values % (dimensions * ...) == 0, "Incorrect number of parameters to deduce dimension.");
	static constexpr auto value = number_of_values / (dimensions * ...);
};

}	// namespace detail

// If element_type is not specified, the type is deduced as common_type.
//
// This has some code duplication (rather than all versions forwarding to the
// explicitly typed overload of make_explicit_array) because every time you
// forward a large number of arguments, gcc and clang both use up a lot of
// memory. For creating an array of around 4000 elements, these functions were
// unusable when they did not construct the array directly.

// These assume that all of the dimensions have been passed in.
template<typename element_type, std::size_t... dimensions, typename... Args>
constexpr auto make_explicit_array(Args && ... args) noexcept {
	return array<element_type, dimensions...>{ std::forward<Args>(args)... };
}
template<std::size_t... dimensions, typename... Args>
constexpr auto make_explicit_array(Args && ... args) noexcept {
	return array<std::common_type_t<Args...>, dimensions...>{ std::forward<Args>(args)... };
}


// These assume you did not specify the inner-most dimension.
template<typename element_type, std::size_t... dimensions, typename... Args>
constexpr auto make_array(Args && ... args) noexcept {
	return array<
		element_type,
		detail::final_dimension<sizeof...(Args), dimensions...>::value, dimensions...
	>{ std::forward<Args>(args)... };
}

template<std::size_t... dimensions, typename... Args>
constexpr auto make_array(Args && ... args) noexcept {
	return array<
		std::common_type_t<Args...>,
		detail::final_dimension<sizeof...(Args), dimensions...>::value, dimensions...
	>{ std::forward<Args>(args)... };
}


}	// namespace containers
