// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/variant/get_index.hpp>

namespace {

template<typename T, int index, typename... Ts>
constexpr bool check_index() {
	static_assert(
		bounded::detail::get_index(
			bounded::constant<index>,
			bounded::type<Ts>...
		) == bounded::constant<index>
	);
	static_assert(
		bounded::detail::get_type(
			bounded::constant<index>,
			bounded::type<Ts>...
		) == bounded::type<T>
	);

	return true;
}

template<typename T, int index, typename... Ts>
constexpr bool check_type_and_index() {
	static_assert(check_index<T, index, Ts...>());
	static_assert(
		bounded::detail::get_index(
			bounded::type<T>,
			bounded::type<Ts>...
		) == bounded::constant<index>
	);
	static_assert(
		bounded::detail::get_type(
			bounded::type<T>,
			bounded::type<Ts>...
		) == bounded::type<T>
	);

	return true;
}

static_assert(check_type_and_index<int, 0, int>());
static_assert(check_type_and_index<int, 0, int, long>());
static_assert(check_type_and_index<int, 1, long, int>());

static_assert(check_index<int, 0, int, int>());
static_assert(check_index<int, 1, int, int>());

} // namespace
