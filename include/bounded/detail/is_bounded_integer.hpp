// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace bounded {

template<auto minimum, auto maximum>
struct integer;

namespace detail {

template<typename T>
inline constexpr auto is_bounded_integer = false;

template<typename T>
inline constexpr auto is_bounded_integer<T const> = is_bounded_integer<T>;

template<typename T>
inline constexpr auto is_bounded_integer<T volatile> = is_bounded_integer<T>;

template<typename T>
inline constexpr auto is_bounded_integer<T const volatile> = is_bounded_integer<T>;

template<typename T>
inline constexpr auto is_bounded_integer<T &> = is_bounded_integer<T>;

template<typename T>
inline constexpr auto is_bounded_integer<T &&> = is_bounded_integer<T>;

template<auto minimum, auto maximum>
inline constexpr auto is_bounded_integer<integer<minimum, maximum>> = true;

} // namespace detail

template<typename T>
concept bounded_integer = detail::is_bounded_integer<T>;

} // namespace bounded
