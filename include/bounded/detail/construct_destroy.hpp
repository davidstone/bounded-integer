// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/addressof.hpp>
#include <bounded/detail/noexcept.hpp>
#include <bounded/detail/requires.hpp>

#include <memory>
#include <type_traits>

namespace bounded {
namespace detail {

// A non-movable type still returns true for is_trivially_copyable and friends
template<typename T>
constexpr auto constexpr_constructible = std::is_move_assignable<T>::value and std::is_trivially_move_assignable<T>::value and std::is_trivially_destructible<T>::value;

}	// namespace detail

// Try () initialization first, then {} initialization

template<typename T, typename... Args, BOUNDED_REQUIRES(!detail::constexpr_constructible<T> and std::is_constructible<T, Args...>::value)>
auto construct(T & ref, Args && ... args) BOUNDED_NOEXCEPT(
	static_cast<void>(::new(static_cast<void *>(::bounded::addressof(ref))) T(std::forward<Args>(args)...))
)
template<typename T, typename... Args, BOUNDED_REQUIRES(detail::constexpr_constructible<T> and std::is_constructible<T, Args...>::value)>
constexpr auto construct(T & ref, Args && ... args) BOUNDED_NOEXCEPT(
	static_cast<void>(ref = T(std::forward<Args>(args)...))
)

template<typename T, typename... Args, BOUNDED_REQUIRES(!detail::constexpr_constructible<T> and !std::is_constructible<T, Args...>::value)>
auto construct(T & ref, Args && ... args) BOUNDED_NOEXCEPT(
	static_cast<void>(::new(static_cast<void *>(::bounded::addressof(ref))) T{std::forward<Args>(args)...})
)
template<typename T, typename... Args, BOUNDED_REQUIRES(detail::constexpr_constructible<T> and !std::is_constructible<T, Args...>::value)>
constexpr auto construct(T & ref, Args && ... args) BOUNDED_NOEXCEPT(
	static_cast<void>(ref = T{std::forward<Args>(args)...})
)

template<typename T, BOUNDED_REQUIRES(!std::is_trivially_destructible<T>::value)>
auto destroy(T & ref) BOUNDED_NOEXCEPT(
	ref.~T()
)
template<typename T, BOUNDED_REQUIRES(std::is_trivially_destructible<T>::value)>
constexpr auto destroy(T &) noexcept {
}

}	// namespace bounded
