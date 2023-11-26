// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.is_bounded_integer;

namespace bounded {

export template<typename T>
constexpr auto is_bounded_integer = false;

template<typename T>
constexpr auto is_bounded_integer<T const> = is_bounded_integer<T>;

template<typename T>
constexpr auto is_bounded_integer<T volatile> = is_bounded_integer<T>;

template<typename T>
constexpr auto is_bounded_integer<T const volatile> = is_bounded_integer<T>;

template<typename T>
constexpr auto is_bounded_integer<T &> = is_bounded_integer<T>;

template<typename T>
constexpr auto is_bounded_integer<T &&> = is_bounded_integer<T>;

} // namespace bounded