// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.lazy_init;

import bounded.concepts;

import std_module;

namespace bounded {

export struct lazy_init_t {
};
export constexpr auto lazy_init = lazy_init_t();

// If your function returns this type, the value will be unwrapped and then
// copied / moved instead of constructed in place. This is necessary for types
// like `std::jthread` that have constructors with unconstrained reference
// parameters where the conversion would not work inside the constructor.
export template<typename T>
struct no_lazy_construction {
	T value;
	constexpr operator T() && {
		return OPERATORS_FORWARD(value);
	}
};
template<typename T>
no_lazy_construction(T) -> no_lazy_construction<T>;

template<typename T>
struct unwrapped_function_result_type {
	using type = T;
};

template<typename T>
struct unwrapped_function_result_type<no_lazy_construction<T>> {
	using type = T;
};

export template<typename Function, typename T>
concept construct_function_for = convertible_to<
	typename unwrapped_function_result_type<std::invoke_result_t<Function>>::type,
	T
>;

} // namespace bounded

template<typename T>
auto function() -> T;
template<typename T>
constexpr auto object = []{ return T(); };

static_assert(bounded::construct_function_for<decltype(function<void>), void>);
static_assert(bounded::construct_function_for<decltype(object<void>), void>);

static_assert(!bounded::construct_function_for<decltype(function<int>), void>);
static_assert(!bounded::construct_function_for<decltype(object<int>), void>);

static_assert(!bounded::construct_function_for<decltype(function<void>), int>);
static_assert(!bounded::construct_function_for<decltype(object<void>), int>);

static_assert(bounded::construct_function_for<decltype(function<int>), int>);
static_assert(bounded::construct_function_for<decltype(object<int>), int>);

static_assert(bounded::construct_function_for<decltype(function<int>), short>);
static_assert(bounded::construct_function_for<decltype(object<int>), short>);

static_assert(bounded::construct_function_for<decltype(function<short>), int>);
static_assert(bounded::construct_function_for<decltype(object<short>), int>);
