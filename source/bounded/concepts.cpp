// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.concepts;

import bounded.declval;
import bounded.function_ptr;

import std_module;

namespace bounded {

export template<typename T, typename... Args>
concept constructible_from =
	(sizeof...(Args) != 1 and requires { T(declval<Args>()...); }) or
	(sizeof...(Args) == 1 and requires { static_cast<T>((..., declval<Args>())); });

export template<typename From, typename To>
concept explicitly_convertible_to = constructible_from<To, From>;

export template<typename From, typename To>
concept convertible_to =
	requires(function_ptr<void, To> function) { function(declval<From>()); } or
	(std::same_as<From, void> and std::same_as<To, void>);

export template<typename T>
concept default_constructible = std::is_default_constructible_v<T>;
export template<typename T>
concept trivially_default_constructible = default_constructible<T> and std::is_trivially_default_constructible_v<T>;

export template<typename T>
concept move_constructible = std::is_move_constructible_v<T>;
export template<typename T>
concept trivially_move_constructible = move_constructible<T> and std::is_trivially_move_constructible_v<T>;

export template<typename T>
concept copy_constructible = std::is_copy_constructible_v<T>;
export template<typename T>
concept trivially_copy_constructible = copy_constructible<T> and std::is_trivially_copy_constructible_v<T>;

export template<typename T>
concept move_assignable = std::is_move_assignable_v<T>;
export template<typename T>
concept trivially_move_assignable = move_assignable<T> and std::is_trivially_move_assignable_v<T>;

export template<typename T>
concept copy_assignable = std::is_copy_assignable_v<T>;
export template<typename T>
concept trivially_copy_assignable = copy_assignable<T> and std::is_trivially_copy_assignable_v<T>;

export template<typename T>
concept trivially_destructible = std::is_trivially_destructible_v<T>;

export template<typename T>
concept trivially_swappable = trivially_move_constructible<T> and trivially_move_assignable<T> and trivially_destructible<T>;

export template<typename T>
concept mostly_trivial =
	trivially_move_constructible<T> and
	trivially_copy_constructible<T> and
	trivially_move_assignable<T> and
	trivially_copy_assignable<T> and
	trivially_destructible<T>;


export template<typename Function, typename Return, typename... Args>
concept invocable_r = requires(Function function) {
	{ function(declval<Args>()...) } -> convertible_to<Return>;
};

} // namespace bounded

static_assert(bounded::constructible_from<void, void>);
static_assert(bounded::constructible_from<int, int>);
static_assert(bounded::constructible_from<void, int>);
static_assert(bounded::constructible_from<int, long>);
static_assert(bounded::constructible_from<long, int>);
static_assert(!bounded::constructible_from<int, void>);
static_assert(!bounded::constructible_from<unsigned long long, unsigned long long *>);
static_assert(!bounded::constructible_from<unsigned long long *, unsigned long long>);
static_assert(!bounded::constructible_from<int *, unsigned *>);

static_assert(bounded::convertible_to<void, void>);
static_assert(bounded::convertible_to<int, int>);
static_assert(!bounded::convertible_to<int, void>);
static_assert(bounded::convertible_to<long, int>);
static_assert(bounded::convertible_to<int, long>);
static_assert(!bounded::convertible_to<void, int>);
static_assert(!bounded::convertible_to<unsigned long long *, unsigned long long>);
static_assert(!bounded::convertible_to<unsigned long long, unsigned long long *>);
static_assert(!bounded::convertible_to<int *, unsigned *>);

static_assert(bounded::invocable_r<void(*)(), void>);
static_assert(!bounded::invocable_r<void(*)(int), void>);
static_assert(bounded::invocable_r<void(*)(int), void, int>);
static_assert(bounded::invocable_r<bool(*)(), bool>);
static_assert(!bounded::invocable_r<bool(*)(int), bool>);
static_assert(bounded::invocable_r<bool(*)(int), bool, int>);
static_assert(bounded::invocable_r<int(*)(unsigned), bool, int>);