// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module tv.tuple_size;

import tv.tuple;

import bounded;
import numeric_traits;
import std_module;

namespace tv {

export template<typename>
extern numeric_traits::incomplete tuple_size;

template<typename T>
concept has_tuple_size = !std::same_as<decltype(tuple_size<T>), numeric_traits::incomplete>;

template<has_tuple_size T>
constexpr auto tuple_size<T const> = tuple_size<T>;

template<has_tuple_size T>
constexpr auto tuple_size<T volatile> = tuple_size<T>;

template<has_tuple_size T>
constexpr auto tuple_size<T const volatile> = tuple_size<T>;

template<has_tuple_size T>
constexpr auto tuple_size<T &> = tuple_size<T>;

template<has_tuple_size T>
constexpr auto tuple_size<T &&> = tuple_size<T>;

template<typename... Ts>
constexpr auto tuple_size<tuple<Ts...>> = bounded::constant<sizeof...(Ts)>;

} // namespace tv
