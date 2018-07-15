// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

#include <type_traits>

namespace containers {
namespace detail {

using namespace bounded::literal;

template<bool, typename... Ts>
union variadic_union {
};

template<typename... Ts>
using variadic_union_t = variadic_union<(... and std::is_trivially_destructible_v<Ts>), Ts...>;


template<typename T, typename... Ts>
union variadic_union<true, T, Ts...> {
	template<typename... Args>
	explicit constexpr variadic_union(bounded::constant_t<0>, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args && ...>):
		head(std::forward<Args>(args)...)
	{
	}
	template<auto n, typename... Args>
	explicit constexpr variadic_union(bounded::constant_t<n> const index, Args && ... args) noexcept(std::is_nothrow_constructible_v<variadic_union_t<Ts...>, Args && ...>):
		tail(index - 1_bi, std::forward<Args>(args)...)
	{
	}

	T head;
	variadic_union_t<Ts...> tail;
};


template<typename T, typename... Ts>
union variadic_union<false, T, Ts...> {
	template<typename... Args>
	explicit constexpr variadic_union(bounded::constant_t<0>, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args && ...>):
		head(std::forward<Args>(args)...)
	{
	}
	template<auto n, typename... Args>
	explicit constexpr variadic_union(bounded::constant_t<n> const index, Args && ... args) noexcept(std::is_nothrow_constructible_v<variadic_union_t<Ts...>, Args && ...>):
		tail(index - 1_bi, std::forward<Args>(args)...)
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



template<typename V, auto n, BOUNDED_REQUIRES(n >= 0_bi)>
constexpr auto && get_union_element(V && v, bounded::constant_t<n> const index) noexcept {
	if constexpr (index == 0_bi) {
		return std::forward<V>(v).head;
	} else {
		return get_union_element(std::forward<V>(v).tail, index - 1_bi);
	}
}

}	// namespace detail
}	// namespace containers
