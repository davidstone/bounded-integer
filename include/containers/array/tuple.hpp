// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/array/array.hpp>

#include <bounded/integer.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace containers {

template<std::size_t index, typename T, std::size_t size>
constexpr auto && get(array<T, size> const & a) {
	return a[bounded::constant<index>];
}
template<std::size_t index, typename T, std::size_t size>
constexpr auto && get(array<T, size> & a) {
	return a[bounded::constant<index>];
}
template<std::size_t index, typename T, std::size_t size>
constexpr auto && get(array<T, size> && a) {
	return std::move(a[bounded::constant<index>]);
}

}	// namespace containers

// I am not sure yet if it is legal to specialize these classes.
#if 0

template<typename T, std::size_t size>
struct std::tuple_size<::containers::array<T, size>> : integral_constant<std::size_t, size> {};

template<std::size_t index, typename T, std::size_t size>
struct std::tuple_element<index, ::containers::array<T, size>> {
	using type = T;
};

#endif

