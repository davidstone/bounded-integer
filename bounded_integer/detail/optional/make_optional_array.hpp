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
#include <utility>

namespace bounded_integer {
namespace detail {

template<typename... Ts>
class common_optional_type;

template<typename... Ts>
using common_optional_type_t = typename common_optional_type<decay_t<Ts>...>::type;

// I manually stamp out a few of these templates to avoid hitting the template
// instantiation limit of various compilers when creating large arrays.
template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename... Ts>
class common_optional_type<T1, T2, T3, T4, T5, T6, T7, T8, T9, Ts...> {
public:
	using type = common_optional_type_t<common_optional_type_t<T1, T2, T3, T4, T5, T6, T7, T8>, T9, Ts...>;
};
template<typename T1, typename T2, typename T3, typename T4, typename T5, typename... Ts>
class common_optional_type<T1, T2, T3, T4, T5, Ts...> {
public:
	using type = common_optional_type_t<common_optional_type_t<T1, T2, T3, T4>, T5, Ts...>;
};
template<typename T1, typename T2, typename... Ts>
class common_optional_type<T1, T2, Ts...> {
public:
	using type = common_optional_type_t<common_optional_type_t<T1, T2>, Ts...>;
};
template<typename T1, typename T2>
class common_optional_type<T1, T2> {
public:
	using type = common_type_t<T1, T2>;
};
template<typename T>
class common_optional_type<T> {
public:
	using type = T;
};
template<typename T1>
class common_optional_type<T1, none_t> {
public:
	using type = T1;
};
template<typename T2>
class common_optional_type<none_t, T2> {
public:
	using type = T2;
};
template<>
class common_optional_type<none_t, none_t> {
public:
	using type = none_t;
};

}	// namespace detail

template<typename... Integers>
constexpr array<optional<detail::common_optional_type_t<Integers...>>, sizeof...(Integers)>
make_optional_array(Integers && ... integers) noexcept {
	return { std::forward<Integers>(integers)... };
}

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_DETAIL_OPTIONAL_MAKE_OPTIONAL_ARRAY_HPP_
