// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.bounded_integer;

import bounded.is_bounded_integer;
import bounded.signed_builtin;
import bounded.unsigned_builtin;

import std_module;

namespace bounded {

export template<typename T>
concept bounded_integer = is_bounded_integer<T>;

export template<typename T>
concept builtin_integer = signed_builtin<T> or unsigned_builtin<T>;

namespace detail {

export template<typename T>
constexpr auto is_integral_constant_of_integral = false;

template<typename T, T value>
constexpr auto is_integral_constant_of_integral<std::integral_constant<T, value>> = builtin_integer<T>;

} // namespace detail
} // namespace bounded

namespace {

struct s {
};

static_assert(!bounded::bounded_integer<int>);
static_assert(!bounded::bounded_integer<bool>);
static_assert(!bounded::bounded_integer<std::byte>);
static_assert(!bounded::bounded_integer<s>);

} // namespace