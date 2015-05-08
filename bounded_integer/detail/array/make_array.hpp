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

#ifndef BOUNDED_INTEGER_DETAIL_MAKE_ARRAY_HPP_
#define BOUNDED_INTEGER_DETAIL_MAKE_ARRAY_HPP_

#include "array.hpp"
#include "../common_type.hpp"
#include <cstddef>
#include <utility>

namespace bounded {
namespace detail {

template<typename T, std::size_t dimension, std::size_t... dimensions>
struct multi_dimensional_array {
	using type = array<typename multi_dimensional_array<T, dimensions...>::type, dimension>;
};

template<typename T, std::size_t dimension>
struct multi_dimensional_array<T, dimension> {
	using type = array<T, dimension>;
};

template<typename T, std::size_t... dimensions>
using multi_dimensional_array_t = typename multi_dimensional_array<T, dimensions...>::type;

template<std::size_t... dimensions>
struct dimension_product {
	static constexpr std::size_t value = 1;
};

template<std::size_t dimension, std::size_t... dimensions>
struct dimension_product<dimension, dimensions...> {
	static constexpr std::size_t value = dimension * dimension_product<dimensions...>::value;
};

template<std::size_t number_of_values, std::size_t... dimensions>
struct final_dimension {
private:
	static constexpr std::size_t product = dimension_product<dimensions...>::value;
public:
	static_assert(number_of_values % product == 0, "Incorrect number of parameters to deduce dimension.");
	static constexpr std::size_t value = number_of_values / product;
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
	return detail::multi_dimensional_array_t<element_type, dimensions...>{ std::forward<Args>(args)... };
}
template<std::size_t... dimensions, typename... Args>
constexpr auto make_explicit_array(Args && ... args) noexcept {
	return detail::multi_dimensional_array_t<std::common_type_t<Args...>, dimensions...>{ std::forward<Args>(args)... };
}


// These assume you did not specify the inner-most dimension.
template<typename element_type, std::size_t... dimensions, typename... Args>
constexpr auto make_array(Args && ... args) noexcept {
	return detail::multi_dimensional_array_t<
		element_type,
		detail::final_dimension<sizeof...(Args), dimensions...>::value, dimensions...
	>{ std::forward<Args>(args)... };
}

template<std::size_t... dimensions, typename... Args>
constexpr auto make_array(Args && ... args) noexcept {
	return detail::multi_dimensional_array_t<
		std::common_type_t<Args...>,
		detail::final_dimension<sizeof...(Args), dimensions...>::value, dimensions...
	>{ std::forward<Args>(args)... };
}


}	// namespace bounded
#endif	// BOUNDED_INTEGER_DETAIL_MAKE_ARRAY_HPP_
