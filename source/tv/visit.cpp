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

template<bool use_index, std::size_t variant_index>
constexpr auto visit_implementation(
	auto && function,
	auto && ... elements
) -> decltype(auto) requires(variant_index == sizeof...(elements)) {
	return OPERATORS_FORWARD(function)(OPERATORS_FORWARD(elements)...);
}

template<bool use_index>
constexpr auto get_element(auto && variant, auto const index) -> decltype(auto) {
	if constexpr (use_index) {
		return indexed_value<
			decltype(OPERATORS_FORWARD(variant)[index]),
			std::size_t(index)
		>(OPERATORS_FORWARD(variant)[index]);
	} else {
		return OPERATORS_FORWARD(variant)[index];
	}
}

template<bool use_index, std::size_t variant_index>
constexpr auto visit_implementation(
	auto && function,
	auto && ... variants
) -> decltype(auto) requires(variant_index < sizeof...(variants)) {
	auto const [...element_indexes] = bounded::index_sequence_struct<variant_index>();
	auto const [...remaining_indexes] = bounded::index_sequence_struct<sizeof...(variants) - variant_index - 1>();
	
	auto const search_index = variants...[variant_index].index().integer();
	using variant_index_t = decltype(search_index);
	static_assert(numeric_traits::min_value<variant_index_t> == 0_bi);
	static constexpr auto indexes = bounded::index_sequence_struct<bounded::number_of<variant_index_t>.value()>();
	template for (constexpr auto index : indexes) {
		if (index == search_index) {
			return ::tv::visit_implementation<use_index, variant_index + 1>(
				OPERATORS_FORWARD(function),
				OPERATORS_FORWARD(variants...[element_indexes.value()])...,
				get_element<use_index>(OPERATORS_FORWARD(variants...[variant_index]), index),
				OPERATORS_FORWARD(variants...[variant_index + 1 + remaining_indexes.value()])...
			);
		}
	}
	std::unreachable();
}

// Accepts any number of variants (including 0) followed by one function
export constexpr auto visit_with_index = []<typename... Args>(Args && ... args) static -> decltype(auto)
	 requires(is_variants_then_visit_function<indexed_variant_types, Args...[sizeof...(args) - 1], Args...>(std::make_index_sequence<sizeof...(Args) - 1>()))
{
	auto [...indexes] = bounded::index_sequence_struct<sizeof...(args) - 1>();
	return ::tv::visit_implementation<true, 0>(
		OPERATORS_FORWARD(args...[sizeof...(args) - 1]),
		OPERATORS_FORWARD(args...[indexes.value()])...
	);
};

// Accepts any number of variants (including 0) followed by one function with
// arity equal to the number of variants
export constexpr auto visit = []<typename... Args>(Args && ... args) static -> decltype(auto)
	requires(is_variants_then_visit_function<variant_types, Args...[sizeof...(args) - 1], Args...>(std::make_index_sequence<sizeof...(Args) - 1>()))
{
	auto [...indexes] = bounded::index_sequence_struct<sizeof...(args) - 1>();
	return ::tv::visit_implementation<false, 0>(
		OPERATORS_FORWARD(args...[sizeof...(args) - 1]),
		OPERATORS_FORWARD(args...[indexes.value()])...
	);
};

} // namespace tv
