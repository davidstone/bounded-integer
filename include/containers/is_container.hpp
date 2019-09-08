// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <array>

namespace containers {

template<typename>
inline constexpr auto is_container = false;

template<typename T>
inline constexpr auto is_container<T[]> = true;

template<typename T, std::size_t size>
inline constexpr auto is_container<T[size]> = true;

template<typename T> requires requires { typename T::allocator_type; }
inline constexpr auto is_container<T> = true;

template<typename T, std::size_t size>
inline constexpr auto is_container<std::array<T, size>> = true;

}	// namespace containers
