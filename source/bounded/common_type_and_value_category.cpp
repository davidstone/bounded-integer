// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.common_type_and_value_category;

import bounded.add_common_cv_reference;

import std_module;

namespace bounded {

template<typename... Ts>
struct common_type_and_value_category_impl;

export template<typename... Ts>
using common_type_and_value_category = typename common_type_and_value_category_impl<Ts...>::type;

template<typename T0>
struct common_type_and_value_category_impl<T0> {
	using type = std::decay_t<T0>;
};

template<typename T0, typename T1>
struct common_type_and_value_category_impl<T0, T1> {
	using type = std::remove_cv_t<
		add_common_cv_reference<
			std::common_type_t<std::decay_t<T0>, std::decay_t<T1>>,
			T0,
			T1
		>
	>;
};

template<>
struct common_type_and_value_category_impl<void, void> {
	using type = void;
};

template<typename T0, typename T1, typename... Ts>
struct common_type_and_value_category_impl<T0, T1, Ts...> {
	using type = common_type_and_value_category<common_type_and_value_category<T0, T1>, Ts...>;
};

} // namespace bounded

namespace {

static_assert(std::same_as<
	bounded::common_type_and_value_category<int>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int const>,
	int
>);



static_assert(std::same_as<
	bounded::common_type_and_value_category<int, int>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int const, int>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int const, int const>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &, int>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int const &, int>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &&, int>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int const &, int const &>,
	int const &
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &, int const &>,
	int const &
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &&, int const &>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &, int &>,
	int &
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &&, int &>,
	int
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &&, int &&>,
	int &&
>);



static_assert(std::same_as<
	bounded::common_type_and_value_category<int, long>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &, long>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int const &, long>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &&, long>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int const &, long const &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &, long const &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &&, long const &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &, long &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &&, long &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<int &&, long &&>,
	long
>);



static_assert(std::same_as<
	bounded::common_type_and_value_category<long &, int>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<long const &, int>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<long &&, int>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<long const &, int const &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<long &, int const &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<long &&, int const &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<long &, int &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<long &&, int &>,
	long
>);

static_assert(std::same_as<
	bounded::common_type_and_value_category<long &&, int &&>,
	long
>);



static_assert(std::same_as<
	bounded::common_type_and_value_category<int, int, int>,
	int
>);

}	// namespace
