// Copyright David Stone 2017.
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
constexpr auto constexpr_constructible = std::is_move_assignable<T>{} and std::is_trivially_move_assignable<T>{} and std::is_trivially_destructible<T>{};

}	// namespace detail

// Try () initialization first, then {} initialization

constexpr struct {
	template<typename T, typename... Args, BOUNDED_REQUIRES(
		not detail::constexpr_constructible<T> and
		std::is_constructible<T, Args...>{}
	)>
	constexpr auto operator()(T & ref, Args && ... args) const BOUNDED_NOEXCEPT_REF(
		*::new(static_cast<void *>(::bounded::addressof(ref))) T(std::forward<Args>(args)...)
	)
	
	// The assignment operator must return a T & if it is trivial
	template<typename T, typename... Args, BOUNDED_REQUIRES(
		detail::constexpr_constructible<T> and
		std::is_constructible<T, Args...>{}
	)>
	constexpr auto operator()(T & ref, Args && ... args) const BOUNDED_NOEXCEPT_REF(
		ref = T(std::forward<Args>(args)...)
	)

	template<typename T, typename... Args, BOUNDED_REQUIRES(
		!detail::constexpr_constructible<T> and
		!std::is_constructible<T, Args...>{}
	)>
	constexpr auto operator()(T & ref, Args && ... args) const BOUNDED_NOEXCEPT_REF(
		*::new(static_cast<void *>(::bounded::addressof(ref))) T{std::forward<Args>(args)...}
	)

	template<typename T, typename... Args, BOUNDED_REQUIRES(
		detail::constexpr_constructible<T> and
		!std::is_constructible<T, Args...>{}
	)>
	constexpr auto operator()(T & ref, Args && ... args) const BOUNDED_NOEXCEPT_REF(
		ref = T{std::forward<Args>(args)...}
	)
} construct;


constexpr struct {
	template<typename T, BOUNDED_REQUIRES(!std::is_trivially_destructible<T>{})>
	constexpr auto operator()(T & ref) const BOUNDED_NOEXCEPT(
		ref.~T()
	)
	template<typename T, BOUNDED_REQUIRES(std::is_trivially_destructible<T>{})>
	constexpr auto operator()(T &) const noexcept {
	}
} destroy;

}	// namespace bounded
