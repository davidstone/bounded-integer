// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module tv.variadic_union;

import bounded;
import std_module;

namespace tv {

export struct uninitialized_union {};

export template<typename... Ts>
union variadic_union {
};

template<typename T, typename... Ts> requires(std::is_trivially_destructible_v<T> and ... and std::is_trivially_destructible_v<Ts>)
union variadic_union<T, Ts...> {
	explicit constexpr variadic_union(bounded::constant_t<0>, auto && construct_):
		head(OPERATORS_FORWARD(construct_)())
	{
	}
	template<auto n>
	explicit constexpr variadic_union(bounded::constant_t<n> const index, auto && construct_):
		tail(index - bounded::constant<1>, OPERATORS_FORWARD(construct_))
	{
	}

	explicit constexpr variadic_union(uninitialized_union) {
	}

	[[no_unique_address]] T head;
	[[no_unique_address]] variadic_union<Ts...> tail;
};


template<typename T, typename... Ts>
union [[clang::trivial_abi]] variadic_union<T, Ts...> {
	explicit constexpr variadic_union(bounded::constant_t<0>, auto && construct_):
		head(OPERATORS_FORWARD(construct_)())
	{
	}
	template<auto n>
	explicit constexpr variadic_union(bounded::constant_t<n> const index, auto && construct_):
		tail(index - bounded::constant<1>, OPERATORS_FORWARD(construct_))
	{
	}

	explicit constexpr variadic_union(uninitialized_union) {
	}

	variadic_union(variadic_union &&) = default;
	variadic_union(variadic_union const &) = default;
	variadic_union & operator=(variadic_union &&) = default;
	variadic_union & operator=(variadic_union const &) = default;
	constexpr ~variadic_union() {}

	[[no_unique_address]] T head;
	[[no_unique_address]] variadic_union<Ts...> tail;
};


export template<auto n> requires(n >= 0)
constexpr auto && get_union_element(auto && v, bounded::constant_t<n> const index) {
	if constexpr (index == bounded::constant<0>) {
		return OPERATORS_FORWARD(v).head;
	} else {
		return get_union_element(OPERATORS_FORWARD(v).tail, index - bounded::constant<1>);
	}
}

} // namespace tv
