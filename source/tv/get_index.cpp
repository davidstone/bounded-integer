// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module tv.get_index;

import tv.nth_type;
import tv.is_valid_index;

import bounded;
import std_module;

namespace tv {

struct get_index_t {
	template<typename Index, typename... Ts> requires matches_exactly_one_type<Index, typename Ts::type...>
	static constexpr auto operator()(bounded::type_t<Index> index, Ts... ts) {
		constexpr auto result = [=] {
			bool found = false;
			std::size_t value = 0;
			(
				...,
				(!found ?
					void(((found = (index == ts)), ++value)) :
					void()
				)
			);
			return value - 1;
		}();
		return bounded::constant<result>;
	}
	template<auto n, typename... Ts> requires variant_integer_index<bounded::constant_t<n>, sizeof...(Ts)>
	static constexpr auto operator()(bounded::constant_t<n> index, Ts...) {
		return index;
	}
};
export constexpr auto get_index = get_index_t();


struct get_type_t {
	template<typename Index, typename... Ts> requires matches_exactly_one_type<Index, typename Ts::type...>
	static constexpr auto operator()(bounded::type_t<Index> index, Ts...) {
		return index;
	}

	template<auto n, typename... Ts> requires variant_integer_index<bounded::constant_t<n>, sizeof...(Ts)>
	static constexpr auto operator()(bounded::constant_t<n>, Ts...) {
		return bounded::type_t<nth_type<n, typename Ts::type...>>();
	}
};
export constexpr auto get_type = get_type_t();

} // namespace tv

template<typename T, int index, typename... Ts>
constexpr bool check_index() {
	static_assert(
		tv::get_index(
			bounded::constant<index>,
			bounded::type<Ts>...
		) == bounded::constant<index>
	);
	static_assert(
		tv::get_type(
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
		tv::get_index(
			bounded::type<T>,
			bounded::type<Ts>...
		) == bounded::constant<index>
	);
	static_assert(
		tv::get_type(
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
