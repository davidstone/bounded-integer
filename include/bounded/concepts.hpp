// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

namespace bounded {

// TODO: File a bug against the standard? The standard version always returns a
// reference, which means it does not work with guaranteed copy elision.
template<typename T>
T declval();

template<typename T> requires std::is_function_v<std::remove_cv_t<T>>
T && declval();

template<typename T, typename... Args>
concept constructible_from = requires { T(declval<Args>()...); };

template<typename From, typename To>
concept explicitly_convertible_to = constructible_from<To, From>;

template<typename From, typename To>
concept convertible_to = explicitly_convertible_to<From, To> and std::is_convertible_v<From, To>;

template<typename T>
concept copy_constructible = std::is_copy_constructible_v<T>;
template<typename T>
concept trivially_copy_constructible = copy_constructible<T> and std::is_trivially_copy_constructible_v<T>;

template<typename T>
concept move_constructible = std::is_move_constructible_v<T>;
template<typename T>
concept trivially_move_constructible = move_constructible<T> and std::is_trivially_move_constructible_v<T>;

template<typename T>
concept copy_assignable = std::is_copy_assignable_v<T>;
template<typename T>
concept trivially_copy_assignable = copy_assignable<T> and std::is_trivially_copy_assignable_v<T>;

template<typename T>
concept move_assignable = std::is_move_assignable_v<T>;
template<typename T>
concept trivially_move_assignable = move_assignable<T> and std::is_trivially_move_assignable_v<T>;

template<typename T>
concept trivially_destructible = std::is_trivially_destructible_v<T>;

} // namespace bounded
