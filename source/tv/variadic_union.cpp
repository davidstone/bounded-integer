// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module tv.variadic_union;

import bounded;
import std_module;

using namespace bounded::literal;

namespace tv {

export struct uninitialized_union {};

export template<typename... Ts>
union variadic_union {
};

template<typename T, typename... Ts>
union [[clang::trivial_abi]] variadic_union<T, Ts...> trivially_relocatable_if_eligible replaceable_if_eligible {
	explicit constexpr variadic_union(bounded::constant_t<0>, auto && construct_):
		head(OPERATORS_FORWARD(construct_)())
	{
	}
	template<auto n>
	explicit constexpr variadic_union(bounded::constant_t<n> const index, auto && construct_):
		tail(index - 1_bi, OPERATORS_FORWARD(construct_))
	{
	}

	explicit constexpr variadic_union(uninitialized_union) {
	}

	variadic_union(variadic_union &&) = default;
	variadic_union(variadic_union const &) = default;
	variadic_union & operator=(variadic_union &&) = default;
	variadic_union & operator=(variadic_union const &) = default;
	~variadic_union() requires(bounded::trivially_destructible<T> and ... and bounded::trivially_destructible<Ts>) = default;
	constexpr ~variadic_union() {}

	template<auto n>
	constexpr auto operator[](this auto && v, bounded::constant_t<n> const index) -> auto && {
		if constexpr (index == 0_bi) {
			return OPERATORS_FORWARD(v).head;
		} else {
			return OPERATORS_FORWARD(v).tail[index - 1_bi];
		}
	}

	[[no_unique_address]] T head;
	[[no_unique_address]] variadic_union<Ts...> tail;
};

} // namespace tv
