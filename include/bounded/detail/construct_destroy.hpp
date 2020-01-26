// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <operators/forward.hpp>
#include <bounded/is_constructible.hpp>
#include <bounded/lazy_init.hpp>

#include <memory>
#include <type_traits>

namespace bounded {
namespace detail {

// A non-movable type still returns true for is_trivially_copyable and friends
template<typename T>
concept constexpr_constructible = std::is_move_assignable_v<T> and std::is_trivially_move_assignable_v<T> and std::is_trivially_destructible_v<T>;

template<typename T, typename... Args>
concept brace_constructible = requires (Args && ... args) { T{OPERATORS_FORWARD(args)...}; };

}	// namespace detail

template<typename T>
inline constexpr auto construct_return = [](auto && ... args) requires constructible_from<T, decltype(args)...> or detail::brace_constructible<T, decltype(args)...> {
	if constexpr (constructible_from<T, decltype(args)...>) {
		return T(OPERATORS_FORWARD(args)...);
	} else {
		return T{OPERATORS_FORWARD(args)...};
	}
};


struct construct_t {
	template<typename T, typename Function>
	constexpr auto & operator()(T & ref, Function && function) const requires construct_function_for<Function, T> {
		if constexpr (detail::constexpr_constructible<T>) {
			return ref = OPERATORS_FORWARD(function)();
		} else {
			return *::new(static_cast<void *>(std::addressof(ref))) T(OPERATORS_FORWARD(function)());
		}
	}
} inline constexpr construct;


inline constexpr auto destroy = [](auto & ref) {
	using T = std::decay_t<decltype(ref)>;
	if constexpr (!std::is_trivially_destructible_v<T>) {
		ref.~T();
	}
};

}	// namespace bounded
