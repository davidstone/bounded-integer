// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/int128.hpp>
#include <bounded/detail/max_builtin.hpp>

#include <type_traits>

namespace bounded {
namespace detail {

struct incomplete;

} // namespace detail

// You cannot `= delete` a variable template
template<typename>
extern detail::incomplete max_value;

template<typename>
extern detail::incomplete min_value;

template<typename T>
inline constexpr auto const & max_value<T const> = max_value<T>;

template<typename T>
inline constexpr auto const & max_value<T volatile> = max_value<T>;

template<typename T>
inline constexpr auto const & max_value<T const volatile> = max_value<T>;

template<typename T>
inline constexpr auto const & min_value<T const> = min_value<T>;

template<typename T>
inline constexpr auto const & min_value<T volatile> = min_value<T>;

template<typename T>
inline constexpr auto const & min_value<T const volatile> = min_value<T>;

template<>
inline constexpr auto max_value<bool> = true;

template<>
inline constexpr auto min_value<bool> = false;

template<detail::unsigned_builtin T>
inline constexpr auto max_value<T> = T(-1);

template<detail::unsigned_builtin T>
inline constexpr auto min_value<T> = T(0);

// Signed integers are two's complement
template<detail::signed_builtin T>
inline constexpr auto max_value<T> = static_cast<T>(max_value<detail::make_unsigned<T>> / 2);

template<detail::signed_builtin T>
inline constexpr auto min_value<T> = static_cast<T>(-max_value<T> - 1);

template<typename T, T value>
inline constexpr auto max_value<std::integral_constant<T, value>> = value;

template<typename T, T value>
inline constexpr auto min_value<std::integral_constant<T, value>> = value;

}	// namespace bounded
