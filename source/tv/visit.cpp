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
	bounded::constant_t<0>,
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
	auto offset,
	auto && ... variants
) -> decltype(auto) requires(variant_index < sizeof...(variants)) {
	auto const search_index = variants...[variant_index].index().integer();
	auto const this_search = search_index - offset;

	auto const [...element_indexes] = bounded::index_sequence_struct<variant_index>();
	auto && variant = OPERATORS_FORWARD(variants...[variant_index]); \
	auto const [...remaining_indexes] = bounded::index_sequence_struct<sizeof...(variants) - variant_index - 1>();
	// Cannot use a lambda because there is no return type that would be valid
	// there. A deduced return type would be potentially void.
	#define VISIT_IMPL(index) \
		do { \
			if constexpr (numeric_traits::max_value<decltype(this_search)> < (index)) { \
				std::unreachable(); \
			} else { \
				return ::tv::visit_implementation<use_index, variant_index + 1>( \
					OPERATORS_FORWARD(function), \
					0_bi, \
					OPERATORS_FORWARD(variants...[element_indexes.value()])..., \
					get_element<use_index>(OPERATORS_FORWARD(variant), offset + (index)), \
					OPERATORS_FORWARD(variants...[variant_index + 1 + remaining_indexes.value()])... \
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
					offset + max_index,
					OPERATORS_FORWARD(variants)...
				);
			}
		}
	}
	#undef VISIT_IMPL
}

// Accepts any number of variants (including 0) followed by one function
export constexpr auto visit_with_index = []<typename... Args>(Args && ... args) static -> decltype(auto)
	 requires(is_variants_then_visit_function<indexed_variant_types, Args...[sizeof...(args) - 1], Args...>(std::make_index_sequence<sizeof...(Args) - 1>()))
{
	auto [...indexes] = bounded::index_sequence_struct<sizeof...(args) - 1>();
	return ::tv::visit_implementation<true, 0>(
		OPERATORS_FORWARD(args...[sizeof...(args) - 1]),
		0_bi,
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
		0_bi,
		OPERATORS_FORWARD(args...[indexes.value()])...
	);
};

} // namespace tv
