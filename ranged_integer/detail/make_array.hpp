// Make an array with automatically deduced size and type
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

#ifndef RANGED_INTEGER_MAKE_ARRAY_HPP_
#define RANGED_INTEGER_MAKE_ARRAY_HPP_

#include "array.hpp"
#include "common_type.hpp"
#include "make_ranged.hpp"
#include <cstddef>
#include <utility>

namespace detail {

template<typename T, std::size_t... dimensions>
class array_c;

template<typename T, std::size_t... dimensions>
using array_type = typename array_c<T, dimensions...>::type;

template<typename T, std::size_t dimension, std::size_t... dimensions>
class array_c<T, dimension, dimensions...> {
public:
	using type = array<array_type<T, dimensions...>, dimension>;
};

template<typename T>
class array_c<T> {
public:
	using type = T;
};

template<std::size_t dimension, std::size_t... dimensions>
class dimension_product {
public:
	static constexpr std::size_t value = dimension * dimension_product<dimensions...>::value;
};
template<std::size_t dimension>
class dimension_product<dimension> {
public:
	static constexpr std::size_t value = dimension;
};

template<std::size_t number_of_integers, std::size_t... dimensions>
class final_dimension {
private:
	static constexpr std::size_t product = dimension_product<dimensions...>::value;
public:
	static_assert(number_of_integers % product == 0, "Not passed enough integers to deduce dimension.");
	static constexpr std::size_t value = number_of_integers / product;
};

}	// namespace detail

template<std::size_t... dimensions>
class multi_array {
public:
	// make_explicit assumes that all of the dimensions have been passed in.
	template<typename... Integers>
	static constexpr auto make_explicit(Integers && ... integers) noexcept {
		using common_t = common_type_t<equivalent_type<Integers>...>;
		return detail::array_type<common_t, dimensions...>{
			static_cast<common_t>(std::forward<Integers>(integers))...
		};
	}

	// make_deduced assumes you did not specify the first dimension.
	template<typename... Integers>
	static constexpr auto make_deduced(Integers && ... integers) noexcept {
		return multi_array<detail::final_dimension<sizeof...(Integers), dimensions...>::value, dimensions...>::make_explicit(std::forward<Integers>(integers)...);
	}
};

template<typename... Integers>
constexpr auto make_array(Integers && ... integers) noexcept {
	return multi_array<sizeof...(Integers)>::make_explicit(std::forward<Integers>(integers)...);
}

#endif	// RANGED_INTEGER_MAKE_ARRAY_HPP_
