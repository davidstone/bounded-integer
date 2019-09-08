// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/forward.hpp>
#include <bounded/detail/noexcept.hpp>

#include <memory>
#include <type_traits>

namespace bounded {
namespace detail {

// A non-movable type still returns true for is_trivially_copyable and friends
template<typename T>
concept constexpr_constructible = std::is_move_assignable_v<T> and std::is_trivially_move_assignable_v<T> and std::is_trivially_destructible_v<T>;

// Try () initialization first, then {} initialization

template<typename T>
struct construct_return_t {
	template<typename... Args> requires std::is_constructible_v<T, Args...>
	constexpr auto operator()(Args && ... args) const BOUNDED_NOEXCEPT_VALUE(
		T(BOUNDED_FORWARD(args)...)
	)
	
	template<typename... Args> requires (not std::is_constructible_v<T, Args...>)
	constexpr auto operator()(Args && ... args) const BOUNDED_NOEXCEPT_VALUE(
		T{BOUNDED_FORWARD(args)...}
	)
};

}	// namespace detail

template<typename T>
inline constexpr auto construct_return = detail::construct_return_t<T>{};


inline constexpr struct construct_t {
	// The assignment operator must return a T & if it is trivial
	template<typename T, typename... Args> requires detail::constexpr_constructible<T>
	constexpr auto operator()(T & ref, Args && ... args) const BOUNDED_NOEXCEPT_REF(
		ref = construct_return<T>(BOUNDED_FORWARD(args)...)
	)

	template<typename T, typename... Args> requires (not detail::constexpr_constructible<T>)
	constexpr auto operator()(T & ref, Args && ... args) const BOUNDED_NOEXCEPT_REF(
		*::new(static_cast<void *>(std::addressof(ref))) T(construct_return<T>(BOUNDED_FORWARD(args)...))
	)
} construct;


inline constexpr auto destroy = [](auto & ref) noexcept {
	using T = std::decay_t<decltype(ref)>;
	if constexpr (!std::is_trivially_destructible_v<T>) {
		ref.~T();
	}
};

}	// namespace bounded
