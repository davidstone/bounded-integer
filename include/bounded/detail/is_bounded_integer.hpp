// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/forward_declaration.hpp>

namespace bounded {
namespace detail {

template<typename T>
inline constexpr auto is_bounded_integer = false;

template<auto minimum, auto maximum, typename overflow_policy>
inline constexpr auto is_bounded_integer<integer<minimum, maximum, overflow_policy>> = true;

template<typename T>
inline constexpr auto is_bounded_integer<T const> = is_bounded_integer<T>;

template<typename T>
inline constexpr auto is_bounded_integer<T volatile> = is_bounded_integer<T>;

template<typename T>
inline constexpr auto is_bounded_integer<T &> = is_bounded_integer<T>;

template<typename T>
inline constexpr auto is_bounded_integer<T &&> = is_bounded_integer<T>;

} // namespace detail

template<typename T>
concept bounded_integer = detail::is_bounded_integer<T>;

} // namespace bounded
