// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/minus.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <type_traits>

namespace bounded {
namespace detail {

template<bool, typename... Ts>
union variadic_union {
};

template<typename... Ts>
using variadic_union_t = variadic_union<(... and std::is_trivially_destructible_v<Ts>), Ts...>;


template<typename T, typename... Ts>
union variadic_union<true, T, Ts...> {
	template<typename... Args>
	explicit constexpr variadic_union(constant_t<0>, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args && ...>):
		head(BOUNDED_FORWARD(args)...)
	{
	}
	template<auto n, typename... Args>
	explicit constexpr variadic_union(constant_t<n> const index, Args && ... args) noexcept(std::is_nothrow_constructible_v<variadic_union_t<Ts...>, Args && ...>):
		tail(index - constant<1>, BOUNDED_FORWARD(args)...)
	{
	}

	T head;
	variadic_union_t<Ts...> tail;
};


template<typename T, typename... Ts>
union variadic_union<false, T, Ts...> {
	template<typename... Args>
	explicit constexpr variadic_union(constant_t<0>, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args && ...>):
		head(BOUNDED_FORWARD(args)...)
	{
	}
	template<auto n, typename... Args>
	explicit constexpr variadic_union(constant_t<n> const index, Args && ... args) noexcept(std::is_nothrow_constructible_v<variadic_union_t<Ts...>, Args && ...>):
		tail(index - constant<1>, BOUNDED_FORWARD(args)...)
	{
	}

	variadic_union(variadic_union &&) = default;
	variadic_union(variadic_union const &) = default;
	variadic_union & operator=(variadic_union &&) = default;
	variadic_union & operator=(variadic_union const &) = default;
	~variadic_union() {}

	T head;
	variadic_union_t<Ts...> tail;
};



template<typename V, auto n> requires(n >= 0)
constexpr auto && get_union_element(V && v, constant_t<n> const index) noexcept {
	if constexpr (index == constant<0>) {
		return BOUNDED_FORWARD(v).head;
	} else {
		return get_union_element(BOUNDED_FORWARD(v).tail, index - constant<1>);
	}
}

}	// namespace detail
}	// namespace bounded
