// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.integral_constant_of_integral;

import bounded.builtin_integer;

import std_module;

namespace bounded {

template<typename T>
constexpr auto is_integral_constant_of_integral = false;

template<typename T, T value>
constexpr auto is_integral_constant_of_integral<std::integral_constant<T, value>> = builtin_integer<T>;

export template<typename T>
concept integral_constant_of_integral = is_integral_constant_of_integral<T>;

} // namespace bounded