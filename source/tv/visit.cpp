// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module tv.visit;

import tv.indexed_value;

import bounded;
import numeric_traits;
import std_module;

namespace tv {

using namespace bounded::literal;

template<typename...>
struct types {
};

template<typename Variant>
using index_sequence_for_variant = std::make_index_sequence<static_cast<std::size_t>(
	numeric_traits::max_value<decltype(bounded::declval<Variant>().index().integer())> + 1_bi
)>;

template<typename Variant, typename Indexes>
struct indexed_variant_types_impl;

template<typename Variant, std::size_t... indexes>
struct indexed_variant_types_impl<Variant, std::index_sequence<indexes...>> {
	using type = types<
		indexed_value<
			decltype(bounded::declval<Variant>()[bounded::constant<indexes>]),
			indexes
		>...
	>;
};

template<typename Variant>
using indexed_variant_types = typename indexed_variant_types_impl<Variant, index_sequence_for_variant<Variant>>::type;


template<typename Variant, typename Indexes>
struct variant_types_impl;

template<typename Variant, std::size_t... indexes>
struct variant_types_impl<Variant, std::index_sequence<indexes...>> {
	using type = types<decltype(bounded::declval<Variant>()[bounded::constant<indexes>])...>;
};

template<typename Variant>
using variant_types = typename variant_types_impl<Variant, index_sequence_for_variant<Variant>>::type;


// The primary template is unused
template<typename Function, typename Args, typename... Lists>
constexpr auto is_cartesian_product_callable = false;

template<typename Function, typename... Args>
constexpr auto is_cartesian_product_callable<Function, types<Args...>> = std::invocable<Function, Args...>;

template<typename Function, typename... Args, typename... Types, typename... Rest>
constexpr auto is_cartesian_product_callable<Function, types<Args...>, types<Types...>, Rest...> = (
	... and
	is_cartesian_product_callable<Function, types<Args..., Types>, Rest...>
);

template<template<typename> typename GetTypes, typename Function, typename... Variants, std::size_t... indexes>
consteval auto is_variants_then_visit_function(std::index_sequence<indexes...>) -> bool {
	return is_cartesian_product_callable<
		Function,
		types<>,
		GetTypes<Variants...[indexes]>...
	>;
}

template<bool use_index, std::size_t, std::size_t... indexes>
constexpr auto visit_implementation(
	auto && function,
	std::index_sequence<indexes...>,
	bounded::constant_t<0>,
	auto && ... variants
) -> decltype(auto) requires(sizeof...(indexes) == sizeof...(variants)) {
	if constexpr (use_index) {
		return OPERATORS_FORWARD(function)(
			indexed_value<
				decltype(OPERATORS_FORWARD(variants)[bounded::constant<indexes>]),
				indexes
			>(OPERATORS_FORWARD(variants)[bounded::constant<indexes>])...
		);
	} else {
		return OPERATORS_FORWARD(function)(
			OPERATORS_FORWARD(variants)[bounded::constant<indexes>]...
		);
	}
}

template<bool use_index, std::size_t variant_index, std::size_t... indexes>
constexpr auto visit_implementation(
	auto && function,
	std::index_sequence<indexes...> initial_indexes,
	auto offset,
	auto && ... variants
) -> decltype(auto) requires(sizeof...(indexes) < sizeof...(variants)) {
	auto const search_index = variants...[variant_index].index().integer();
	auto const this_search = search_index - offset;

	// Cannot use a lambda because there is no return type that would be valid
	// there. A deduced return type would be potentially void.
	#define VISIT_IMPL(index) \
		do { \
			if constexpr (numeric_traits::max_value<decltype(this_search)> < (index)) { \
				std::unreachable(); \
			} else { \
				return ::tv::visit_implementation<use_index, variant_index + 1>( \
					OPERATORS_FORWARD(function), \
					std::index_sequence<indexes..., static_cast<std::size_t>(offset + (index))>{}, \
					0_bi, \
					OPERATORS_FORWARD(variants)... \
				); \
			} \
		} while (false)

	// 16 is arbitrary
	switch (this_search.value()) {
		case 0: VISIT_IMPL(0_bi);
		case 1: VISIT_IMPL(1_bi);
		case 2: VISIT_IMPL(2_bi);
		case 3: VISIT_IMPL(3_bi);
		case 4: VISIT_IMPL(4_bi);
		case 5: VISIT_IMPL(5_bi);
		case 6: VISIT_IMPL(6_bi);
		case 7: VISIT_IMPL(7_bi);
		case 8: VISIT_IMPL(8_bi);
		case 9: VISIT_IMPL(9_bi);
		case 10: VISIT_IMPL(10_bi);
		case 11: VISIT_IMPL(11_bi);
		case 12: VISIT_IMPL(12_bi);
		case 13: VISIT_IMPL(13_bi);
		case 14: VISIT_IMPL(14_bi);
		case 15: VISIT_IMPL(15_bi);
		default: {
			constexpr auto max_index = 16_bi;
			if constexpr (numeric_traits::max_value<decltype(this_search)> < max_index) {
				std::unreachable();
			} else {
				return ::tv::visit_implementation<use_index, variant_index>(
					OPERATORS_FORWARD(function),
					initial_indexes,
					offset + max_index,
					OPERATORS_FORWARD(variants)...
				);
			}
		}
	}
	#undef VISIT_IMPL
}

// Often, we want to write a function that accepts a variadic number of
// arguments and a function. We would like to accept the function parameter
// last, since that gives the best syntax when passing a lambda. However, you
// cannot put something after a variadic pack, therefore the function is part of
// the pack. rotate_transform allows us to write an interface that accepts stuff
// and a function to operate on the stuff, but allows us to decide whether to
// pass values or indexed values to the user's function.
template<bool use_index, std::size_t... indexes>
constexpr auto rotate_transform(auto && function, std::index_sequence<indexes...>, auto && ... args) -> decltype(auto) {
	return ::tv::visit_implementation<use_index, 0>(
		OPERATORS_FORWARD(function),
		std::index_sequence<>(),
		0_bi,
		OPERATORS_FORWARD(args...[indexes])...
	);
}

// Accepts any number of variants (including 0) followed by one function
export constexpr auto visit_with_index = []<typename... Args>(Args && ... args) static -> decltype(auto)
	 requires(is_variants_then_visit_function<indexed_variant_types, Args...[sizeof...(args) - 1], Args...>(std::make_index_sequence<sizeof...(Args) - 1>()))
{
	return ::tv::rotate_transform<true>(
		OPERATORS_FORWARD(args...[sizeof...(args) - 1]),
		std::make_index_sequence<sizeof...(args) - 1>(),
		OPERATORS_FORWARD(args)...
	);
};

// Accepts any number of variants (including 0) followed by one function with
// arity equal to the number of variants
export constexpr auto visit = []<typename... Args>(Args && ... args) static -> decltype(auto)
	requires(is_variants_then_visit_function<variant_types, Args...[sizeof...(args) - 1], Args...>(std::make_index_sequence<sizeof...(Args) - 1>()))
{
	return ::tv::rotate_transform<false>(
		OPERATORS_FORWARD(args...[sizeof...(args) - 1]),
		std::make_index_sequence<sizeof...(args) - 1>(),
		OPERATORS_FORWARD(args)...
	);
};

} // namespace tv
