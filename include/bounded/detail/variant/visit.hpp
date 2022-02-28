// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/literal.hpp>
#include <bounded/detail/type.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <type_traits>

namespace bounded {
namespace detail {

template<typename, auto>
concept any_with_value = true;

// Often, we want to write a function that accepts a variadic number of
// arguments and a function. We would like to accept the function parameter
// last, since that gives the best syntax when passing a lamba. However, you
// cannot put something after a variadic pack, therefore the function is part of
// the pack. reorder_transform accepts a parameter pack, followed by a function,
// followed by another function. The final function is called with the penultimate
// function as the first parameter, then all remaining arguments in their
// original order after that. This allows us to write an interface that accepts
// stuff and a function to operate on the stuff, but allows us to transform the
// arguments in some way before calling the user function.
constexpr decltype(auto) reorder_transform(auto transform, auto && ... args) {
	return [&]<std::size_t... indexes>(std::index_sequence<indexes...>) {
		return [&](any_with_value<indexes> auto && ... non_function_args, auto && function) {
			return transform(OPERATORS_FORWARD(function), OPERATORS_FORWARD(non_function_args)...);
		}(OPERATORS_FORWARD(args)...);
	}(std::make_index_sequence<sizeof...(args) - 1>());
};

}	// namespace detail

template<typename T, std::size_t n>
struct visitor_parameter {
	static_assert(std::is_reference_v<T>);
	T value;
	static constexpr auto index = constant<n>;
};

namespace detail {

template<typename Variant, typename Indexes>
struct variant_types_impl;

template<typename Variant, std::size_t... indexes>
struct variant_types_impl<Variant, std::index_sequence<indexes...>> {
	using type = types<
		visitor_parameter<
			decltype(std::declval<Variant>()[bounded::constant<indexes>]),
			indexes
		>...
	>;
};

template<typename Variant>
using variant_types = typename variant_types_impl<
	Variant,
	std::make_index_sequence<static_cast<std::size_t>(
		numeric_traits::max_value<decltype(std::declval<Variant>().index())> + constant<1>
	)>
>::type;


// The primary template is unused
template<typename Function, typename Args, typename... Lists>
inline constexpr auto is_cartesian_product_callable = false;

template<typename Function, typename... Args>
inline constexpr auto is_cartesian_product_callable<Function, types<Args...>> = std::is_invocable_v<Function, Args...>;

template<typename Function, typename... Args, typename... Types, typename... Rest>
inline constexpr auto is_cartesian_product_callable<Function, types<Args...>, types<Types...>, Rest...> = (
	... and
	is_cartesian_product_callable<Function, types<Args..., Types>, Rest...>
);

// The primary template just rotates the parameters
template<std::size_t index, typename TransformFunction, typename Arg, typename... Args>
inline constexpr auto is_variants_then_visit_function = is_variants_then_visit_function<
	index - 1U,
	TransformFunction,
	Args...,
	Arg
>;

// The specialization forwards to the implementation that does the real work
template<typename TransformFunction, typename Function, typename... Variants>
inline constexpr auto is_variants_then_visit_function<
	0U,
	TransformFunction,
	Function,
	Variants...
> = is_cartesian_product_callable<
	decltype(std::declval<TransformFunction>()(std::declval<Function>())),
	types<>,
	variant_types<Variants>...
>;




template<std::size_t... indexes>
constexpr decltype(auto) visit_implementation(
	auto && function,
	std::index_sequence<indexes...>,
	bounded::constant_t<0>,
	auto && ... variants
) requires(sizeof...(indexes) == sizeof...(variants)) {
	return OPERATORS_FORWARD(function)(
		visitor_parameter<
			decltype(OPERATORS_FORWARD(variants)[bounded::constant<indexes>]),
			indexes
		>{OPERATORS_FORWARD(variants)[bounded::constant<indexes>]}...
	);
}

// This function accepts the pack of all variants twice. It passes over them
// once to get all the indexes, then again to pull out the values.
template<std::size_t... indexes>
constexpr decltype(auto) visit_implementation(
	auto && function,
	std::index_sequence<indexes...> initial_indexes,
	auto offset,
	auto const & variant,
	auto && ... variants
) requires(sizeof...(indexes) < sizeof...(variants)) {
	auto const search_index = variant.index();

	// Cannot use a lambda because there is no return type that would be valid
	// there. A deduced return type would be potentially void.
	#define BOUNDED_DETAIL_VISIT_IMPL(index) \
		do { \
			if constexpr (numeric_traits::max_value<decltype(search_index)> < (index)) { \
				bounded::unreachable(); \
			} else { \
				return ::bounded::detail::visit_implementation( \
					OPERATORS_FORWARD(function), \
					std::index_sequence<indexes..., static_cast<std::size_t>(offset + (index))>{}, \
					0_bi, \
					OPERATORS_FORWARD(variants)... \
				); \
			} \
		} while (false)

	// 16 is arbitrary
	switch ((search_index - offset).value()) {
		case 0: BOUNDED_DETAIL_VISIT_IMPL(0_bi);
		case 1: BOUNDED_DETAIL_VISIT_IMPL(1_bi);
		case 2: BOUNDED_DETAIL_VISIT_IMPL(2_bi);
		case 3: BOUNDED_DETAIL_VISIT_IMPL(3_bi);
		case 4: BOUNDED_DETAIL_VISIT_IMPL(4_bi);
		case 5: BOUNDED_DETAIL_VISIT_IMPL(5_bi);
		case 6: BOUNDED_DETAIL_VISIT_IMPL(6_bi);
		case 7: BOUNDED_DETAIL_VISIT_IMPL(7_bi);
		case 8: BOUNDED_DETAIL_VISIT_IMPL(8_bi);
		case 9: BOUNDED_DETAIL_VISIT_IMPL(9_bi);
		case 10: BOUNDED_DETAIL_VISIT_IMPL(10_bi);
		case 11: BOUNDED_DETAIL_VISIT_IMPL(11_bi);
		case 12: BOUNDED_DETAIL_VISIT_IMPL(12_bi);
		case 13: BOUNDED_DETAIL_VISIT_IMPL(13_bi);
		case 14: BOUNDED_DETAIL_VISIT_IMPL(14_bi);
		case 15: BOUNDED_DETAIL_VISIT_IMPL(15_bi);
		default: {
			constexpr auto max_index = 16_bi;
			if constexpr (numeric_traits::max_value<decltype(search_index)> < max_index) {
				bounded::unreachable();
			} else {
				return ::bounded::detail::visit_implementation(
					OPERATORS_FORWARD(function),
					initial_indexes,
					offset + max_index,
					variant,
					OPERATORS_FORWARD(variants)...
				);
			}
		}
	}
	#undef BOUNDED_DETAIL_VISIT_IMPL
}

inline constexpr auto visit_interface = [](auto transform) {
	return [=](auto && function, auto && ... variants) {
		return ::bounded::detail::visit_implementation(
			transform(OPERATORS_FORWARD(function)),
			std::index_sequence<>{},
			0_bi,
			variants...,
			OPERATORS_FORWARD(variants)...
		);
	};
};

} // namespace detail

inline constexpr struct visit_with_index_t {
private:
	static inline constexpr auto identity = [](auto && function) -> decltype(auto) {
		return OPERATORS_FORWARD(function);
	};
public:
	// Any number of variants (including 0) followed by one function
	template<typename... Args> requires detail::is_variants_then_visit_function<sizeof...(Args) - 1U, decltype(identity), Args...>
	constexpr decltype(auto) operator()(Args && ... args) const {
		return detail::reorder_transform(detail::visit_interface(identity), OPERATORS_FORWARD(args)...);
	}
} visit_with_index;

// Accepts any number of variants (including 0) followed by one function with
// arity equal to the number of variants
inline constexpr struct visit_t {
private:
	// The reference is safe in the inner lambda because this function lives
	// only for the duration of the call. Do not use a lambda to work around
	// https://bugs.llvm.org/show_bug.cgi?id=42749
	template<typename Function>
	struct unwrap_visitor_parameter {
		Function && function;
		constexpr auto operator()(auto && ... args) && OPERATORS_RETURNS(
			OPERATORS_FORWARD(function)(OPERATORS_FORWARD(args).value...)
		)
	};

	static inline constexpr auto get_value_only = [](auto && function) {
		return unwrap_visitor_parameter<decltype(function)>{OPERATORS_FORWARD(function)};
	};
public:
	template<typename... Args> requires detail::is_variants_then_visit_function<sizeof...(Args) - 1U, decltype(get_value_only), Args...>
	constexpr decltype(auto) operator()(Args && ... args) const {
		return detail::reorder_transform(detail::visit_interface(get_value_only), OPERATORS_FORWARD(args)...);
	}
} visit;

}	// namespace bounded
