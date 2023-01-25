// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module bounded.construct_destroy;

import bounded.concepts;
import bounded.lazy_init;

import std_module;

namespace bounded {

// https://quuxplusone.github.io/blog/2018/05/17/super-elider-round-2/
template<typename T, typename Function>
struct superconstructing_super_elider {
	constexpr explicit superconstructing_super_elider(Function function):
		m_function(OPERATORS_FORWARD(function))
	{
	}

	// Deleting the move constructor limits the cases where this will silently
	// collide with an "accepts anything" constructor. For instance, this class
	// can be used with std::any.
	superconstructing_super_elider(superconstructing_super_elider &&) = delete;

	constexpr operator T() && {
		return static_cast<T>(OPERATORS_FORWARD(m_function)());
	}

private:
	Function m_function;
};

template<typename T, typename... Args>
concept brace_constructible = requires (Args && ... args) { T{OPERATORS_FORWARD(args)...}; };

export template<typename T>
concept non_const = !std::is_const_v<T>;

// TODO: Use lambda https://github.com/llvm/llvm-project/issues/59513
template<typename T>
struct construct_t {
	static constexpr auto operator()(auto && ... args) -> T requires constructible_from<T, decltype(args)...> or brace_constructible<T, decltype(args)...> {
		if constexpr (constructible_from<T, decltype(args)...>) {
			return T(OPERATORS_FORWARD(args)...);
		} else {
			return T{OPERATORS_FORWARD(args)...};
		}
	}
};
export template<typename T>
constexpr auto construct = construct_t<T>();

template<typename T>
constexpr auto is_no_lazy_construction = false;

template<typename T>
constexpr auto is_no_lazy_construction<no_lazy_construction<T>> = true;

export constexpr auto construct_at = []<non_const T, construct_function_for<T> Function>(T & ref, Function && function) -> T & {
	auto make = [&] {
		if constexpr (is_no_lazy_construction<std::invoke_result_t<Function>>) {
			return OPERATORS_FORWARD(function)().value;
		} else {
			return superconstructing_super_elider<T, Function>(OPERATORS_FORWARD(function));
		}
	};
	return *std::construct_at(std::addressof(ref), make());
};

export constexpr auto destroy = [](non_const auto & ref) -> void {
	std::destroy_at(std::addressof(ref));
};

} // namespace bounded

struct empty {};

struct accepts_anything {
	template<typename T>
	explicit constexpr accepts_anything(T &&) {
		static_assert(std::same_as<T, int>);
	}
};

union u {
	constexpr u():
		e()
	{
	}
	empty e;
	accepts_anything a;
};

static_assert([]{
	auto x = u();
	bounded::construct_at(x.a, [] { return bounded::no_lazy_construction(accepts_anything(5)); });
	return true;
}());