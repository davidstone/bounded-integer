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
#include "make_ranged.hpp"
#include <array>
#include <cstddef>
#include <utility>

namespace detail {

template<std::size_t... integers>
class integer_sequence{};

template<typename T1, typename T2>
class concatenate;

template<std::size_t... lhs, std::size_t... rhs>
class concatenate<integer_sequence<lhs...>, integer_sequence<rhs...>> {
public:
	using type = integer_sequence<lhs..., rhs...>;
};

template<typename T1, typename T2>
using concatenate_t = typename concatenate<T1, T2>::type;

template<std::size_t min, std::size_t max>
class make_integer_sequence_c;
template<std::size_t min, std::size_t max>
using make_integer_sequence = typename make_integer_sequence_c<min, max>::type;

template<std::size_t min, std::size_t max>
class make_integer_sequence_c {
private:
	// avoid overflow, although the compiler would probably run out of memory
	// if it mattered
	static constexpr std::size_t midpoint = min + (max - min) / 2;
public:
	static_assert(min < max, "Range inverted.");
	using type = concatenate_t<make_integer_sequence<min, midpoint>, make_integer_sequence<midpoint + 1, max>>;
};

template<std::size_t value>
class make_integer_sequence_c<value, value> {
public:
	using type = integer_sequence<value>;
};


// This gets the variadic parameter at position index
template<std::size_t index>
class variadic {
public:
	template<typename Integer, typename... Integers>
	static constexpr auto get(Integer &&, Integers && ... integers) noexcept -> decltype(variadic<index - 1>::get(std::forward<Integers>(integers)...)) {
		return variadic<index - 1>::get(std::forward<Integers>(integers)...);
	}
};
template<>
class variadic<0> {
public:
	template<typename Integer, typename... Integers>
	static constexpr Integer && get(Integer && integer, Integers && ...) noexcept {
		return std::forward<Integer>(integer);
	}
};

// No deduced size
template<typename T, std::size_t... dimensions>
class multi_array;

// A lot of this is greatly simplified in C++14 with relaxed rules for braces
template<typename T, std::size_t dimension, std::size_t... dimensions>
class multi_array<T, dimension, dimensions...> {
private:
public:
	template<std::size_t... indexes, typename... Integers>
	static constexpr typename multi_array<T, dimensions...>::type apply_n(integer_sequence<indexes...>, Integers && ... integers) noexcept {
		return multi_array<T, dimensions...>::make(variadic<indexes>::get(std::forward<Integers>(integers)...)...);
	}

	using type = std::array<typename multi_array<T, dimensions...>::type, dimension>;

	
	// Break the variadic pack into `dimension` chunks. Construct `dimension`
	// nested types with each chunk.
	
	template<std::size_t... indexes, typename... Integers>
	static constexpr type make_helper(integer_sequence<indexes...>, Integers && ... integers) noexcept {
		return type{{
			apply_n(
				make_integer_sequence<
					indexes * (sizeof...(Integers) / dimension),
					(indexes + 1) * (sizeof...(Integers) / dimension) - 1
				>{},
				std::forward<Integers>(integers)...
			// This final ... should expand `indexes`
			)...
		}};
	}
	template<typename... Integers>
	static constexpr type make(Integers && ... integers) noexcept {
		return make_helper(make_integer_sequence<0, dimension - 1>{}, std::forward<Integers>(integers)...);
	}
};

template<typename T>
class multi_array<T> {
public:
	using type = T;
	static constexpr type make(T && integer) noexcept {
		return static_cast<type>(std::forward<T>(integer));
	}
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
	// make_deduced assumes you did not specify the first dimension.
	template<
		typename... Integers,
		typename common_t = common_type_t<equivalent_type<Integers>...>,
		typename result_type = detail::multi_array<common_t, dimensions...>
	>
	static constexpr typename result_type::type make_explicit(Integers && ... integers) noexcept {
		return result_type::make(std::forward<Integers>(integers)...);
	}

	template<typename... Integers>
	static constexpr auto make_deduced(Integers && ... integers) noexcept -> decltype(multi_array<detail::final_dimension<sizeof...(Integers), dimensions...>::value, dimensions...>::make_explicit(std::forward<Integers>(integers)...)) {
		return multi_array<detail::final_dimension<sizeof...(Integers), dimensions...>::value, dimensions...>::make_explicit(std::forward<Integers>(integers)...);
	}
};

template<typename... Integers>
constexpr auto make_ranged_array(Integers && ... integers) noexcept -> decltype(multi_array<sizeof...(Integers)>::make_explicit(std::forward<Integers>(integers)...)) {
	return multi_array<sizeof...(Integers)>::make_explicit(std::forward<Integers>(integers)...);
}

template<intmax_t... integers>
constexpr auto make_ranged_array() noexcept -> decltype(make_ranged_array(make_ranged<integers>()...)) {
	return make_ranged_array(make_ranged<integers>()...);
}

#endif	// RANGED_INTEGER_MAKE_RANGED_ARRAY_HPP_
