// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <operators/forward.hpp>
#include <bounded/concepts.hpp>
#include <bounded/lazy_init.hpp>

#include <memory>
#include <type_traits>

namespace bounded {
namespace detail {

// https://quuxplusone.github.io/blog/2018/05/17/super-elider-round-2/
template<typename Function>
struct superconstructing_super_elider {
	constexpr explicit superconstructing_super_elider(Function function):
		m_function(std::move(function))
	{
	}

	// Deleting the move constructor limits the cases where this will silently
	// collide with an "accepts anything" constructor. For instance, this class
	// can be used with std::any.
	superconstructing_super_elider(superconstructing_super_elider &&) = delete;

	constexpr operator auto() && {
		return std::move(m_function)();
	}

private:
	Function m_function;
};

template<typename T, typename... Args>
concept brace_constructible = requires (Args && ... args) { T{OPERATORS_FORWARD(args)...}; };

} // namespace detail

template<typename T>
inline constexpr auto construct_return = [](auto && ... args) requires constructible_from<T, decltype(args)...> or detail::brace_constructible<T, decltype(args)...> {
	if constexpr (constructible_from<T, decltype(args)...>) {
		return T(OPERATORS_FORWARD(args)...);
	} else {
		return T{OPERATORS_FORWARD(args)...};
	}
};


inline constexpr auto construct = []<typename T>(T & ref, construct_function_for<T> auto && function) -> auto & {
	return *std::construct_at(
		std::addressof(ref),
		detail::superconstructing_super_elider([&] { return static_cast<T>(OPERATORS_FORWARD(function)()); })
	);
};

inline constexpr auto destroy = [](auto & ref) {
	using T = std::decay_t<decltype(ref)>;
	if constexpr (!std::is_trivially_destructible_v<T>) {
		ref.~T();
	}
};

} // namespace bounded