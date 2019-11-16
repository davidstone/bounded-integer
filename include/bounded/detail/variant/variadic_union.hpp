// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/operators.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <type_traits>

namespace bounded {
namespace detail {

template<typename... Ts>
union variadic_union {
};

template<typename T, typename... Ts> requires(std::is_trivially_destructible_v<T> and ... and std::is_trivially_destructible_v<Ts>)
union variadic_union<T, Ts...> {
	explicit constexpr variadic_union(constant_t<0>, auto && ... args):
		head(BOUNDED_FORWARD(args)...)
	{
	}
	template<auto n>
	explicit constexpr variadic_union(constant_t<n> const index, auto && ... args):
		tail(index - constant<1>, BOUNDED_FORWARD(args)...)
	{
	}

	T head;
	variadic_union<Ts...> tail;
};


template<typename T, typename... Ts>
union variadic_union<T, Ts...> {
	explicit constexpr variadic_union(constant_t<0>, auto && ... args):
		head(BOUNDED_FORWARD(args)...)
	{
	}
	template<auto n>
	explicit constexpr variadic_union(constant_t<n> const index, auto && ... args):
		tail(index - constant<1>, BOUNDED_FORWARD(args)...)
	{
	}

	variadic_union(variadic_union &&) = default;
	variadic_union(variadic_union const &) = default;
	variadic_union & operator=(variadic_union &&) = default;
	variadic_union & operator=(variadic_union const &) = default;
	~variadic_union() {}

	T head;
	variadic_union<Ts...> tail;
};



template<auto n> requires(n >= 0)
constexpr auto && get_union_element(auto && v, constant_t<n> const index) {
	if constexpr (index == constant<0>) {
		return BOUNDED_FORWARD(v).head;
	} else {
		return get_union_element(BOUNDED_FORWARD(v).tail, index - constant<1>);
	}
}

}	// namespace detail
}	// namespace bounded
