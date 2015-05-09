// Make an array of optional<T> with deduced size and type
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

#ifndef BOUNDED_INTEGER_DETAIL_OPTIONAL_MAKE_OPTIONAL_ARRAY_HPP_
#define BOUNDED_INTEGER_DETAIL_OPTIONAL_MAKE_OPTIONAL_ARRAY_HPP_

#include "optional.hpp"
#include "../common_type.hpp"
#include "../array/array.hpp"
#include "../array/make_array.hpp"

#include <type_traits>
#include <utility>

namespace bounded {
namespace detail {

template<typename... Ts>
struct common_optional_type;

template<typename... Ts>
using common_optional_type_t = typename common_optional_type<std::decay_t<Ts>...>::type;

// I manually stamp out a few of these templates to avoid hitting the template
// instantiation limit of various compilers when creating large arrays.
template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename... Ts>
struct common_optional_type<T1, T2, T3, T4, T5, T6, T7, T8, T9, Ts...> {
	using type = common_optional_type_t<common_optional_type_t<T1, T2, T3, T4, T5, T6, T7, T8>, T9, Ts...>;
};
template<typename T1, typename T2, typename T3, typename T4, typename T5, typename... Ts>
struct common_optional_type<T1, T2, T3, T4, T5, Ts...> {
	using type = common_optional_type_t<common_optional_type_t<T1, T2, T3, T4>, T5, Ts...>;
};
template<typename T1, typename T2, typename... Ts>
struct common_optional_type<T1, T2, Ts...> {
	using type = common_optional_type_t<common_optional_type_t<T1, T2>, Ts...>;
};
template<typename T1, typename T2>
struct common_optional_type<T1, T2> {
	using type = std::common_type_t<T1, T2>;
};
template<typename T>
struct common_optional_type<T> {
	using type = T;
};
template<typename T1>
struct common_optional_type<T1, none_t> {
	using type = T1;
};
template<typename T2>
struct common_optional_type<none_t, T2> {
	using type = T2;
};
template<>
struct common_optional_type<none_t, none_t> {
	using type = none_t;
};

}	// namespace detail

// This assumes that all of the dimensions have been passed in.
template<std::size_t... dimensions, typename... Integers>
constexpr auto make_explicit_optional_array(Integers && ... integers) noexcept {
	return array<
		optional<detail::common_optional_type_t<Integers...>>,
		dimensions...
	>{ std::forward<Integers>(integers)... };
}


// This assumes you did not specify the first dimension.
template<std::size_t... dimensions, typename... Integers>
constexpr auto make_optional_array(Integers && ... integers) noexcept {
	return array<
		optional<detail::common_optional_type_t<Integers...>>,
		detail::final_dimension<sizeof...(Integers), dimensions...>::value, dimensions...
	>{ std::forward<Integers>(integers)... };
}

}	// namespace bounded
#endif	// BOUNDED_INTEGER_DETAIL_OPTIONAL_MAKE_OPTIONAL_ARRAY_HPP_
