// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <concepts>
#include <type_traits>

namespace bounded {

template<typename Result, typename... Args>
using function_ptr = Result(*)(Args...);

// TODO: File a bug against the standard? The standard version always returns a
// reference, which means it does not work with guaranteed copy elision.
template<typename T>
auto declval() -> T;

template<typename T> requires std::is_function_v<std::remove_cv_t<T>>
auto declval() -> T &&;

template<typename T, typename... Args>
concept constructible_from =
	(sizeof...(Args) != 1 and requires { T(declval<Args>()...); }) or
	(sizeof...(Args) == 1 and requires { static_cast<T>((..., declval<Args>())); });

template<typename From, typename To>
concept explicitly_convertible_to = constructible_from<To, From>;

template<typename From, typename To>
concept convertible_to =
	requires(function_ptr<void, To> function) { function(declval<From>()); } or
	(std::same_as<From, void> and std::same_as<To, void>);

template<typename T>
concept move_constructible = std::is_move_constructible_v<T>;
template<typename T>
concept trivially_move_constructible = move_constructible<T> and std::is_trivially_move_constructible_v<T>;

template<typename T>
concept copy_constructible = std::is_copy_constructible_v<T>;
template<typename T>
concept trivially_copy_constructible = copy_constructible<T> and std::is_trivially_copy_constructible_v<T>;

template<typename T>
concept move_assignable = std::is_move_assignable_v<T>;
template<typename T>
concept trivially_move_assignable = move_assignable<T> and std::is_trivially_move_assignable_v<T>;

template<typename T>
concept copy_assignable = std::is_copy_assignable_v<T>;
template<typename T>
concept trivially_copy_assignable = copy_assignable<T> and std::is_trivially_copy_assignable_v<T>;

template<typename T>
concept trivially_swappable = std::is_trivially_copyable_v<T> and std::is_trivially_copy_assignable_v<T> and std::is_trivially_destructible_v<T>;

} // namespace bounded
