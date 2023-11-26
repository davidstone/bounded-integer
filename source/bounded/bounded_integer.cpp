// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.bounded_integer;

import bounded.is_bounded_integer;
import bounded.signed_builtin;

import numeric_traits;
import std_module;

namespace bounded {

export template<typename T>
concept bounded_integer = is_bounded_integer<T>;

export template<typename T>
concept unsigned_builtin = numeric_traits::unsigned_builtin<T> and !std::same_as<T, bool>;

export template<typename T>
concept builtin_integer = signed_builtin<T> or unsigned_builtin<T>;

namespace detail {

export template<typename T>
constexpr auto is_integral_constant_of_integral = false;

template<typename T, T value>
constexpr auto is_integral_constant_of_integral<std::integral_constant<T, value>> = builtin_integer<T>;

} // namespace detail

export template<typename T>
concept integral = builtin_integer<T> or bounded_integer<T> or detail::is_integral_constant_of_integral<T>;

export template<typename T>
concept isomorphic_to_integral = integral<T> or std::is_enum_v<T> or std::same_as<T, bool>;

} // namespace bounded

namespace {

struct s {
};

static_assert(!bounded::bounded_integer<int>);
static_assert(!bounded::bounded_integer<bool>);
static_assert(!bounded::bounded_integer<std::byte>);
static_assert(!bounded::bounded_integer<s>);

static_assert(bounded::integral<int>);
static_assert(!bounded::integral<bool>);
static_assert(!bounded::integral<std::byte>);
static_assert(!bounded::integral<s>);

static_assert(bounded::isomorphic_to_integral<int>);
static_assert(bounded::isomorphic_to_integral<bool>);
static_assert(bounded::isomorphic_to_integral<std::byte>);
static_assert(!bounded::isomorphic_to_integral<s>);

} // namespace