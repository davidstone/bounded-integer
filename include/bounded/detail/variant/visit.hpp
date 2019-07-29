// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/type.hpp>

#include <type_traits>

namespace bounded {
namespace detail {

template<typename TransformFunction, typename Function, typename... Args>
constexpr decltype(auto) reorder_transform_implementation(
	bounded::constant_t<0>,
	TransformFunction transform,
	Function && function,
	Args && ... args
) {
	return transform(BOUNDED_FORWARD(function), BOUNDED_FORWARD(args)...);
}
template<auto index_, typename TransformFunction, typename Arg, typename... Args>
constexpr decltype(auto) reorder_transform_implementation(bounded::constant_t<index_> index, TransformFunction transform, Arg && arg, Args && ... args) {
	return ::bounded::detail::reorder_transform_implementation(index - 1_bi, transform, BOUNDED_FORWARD(args)..., BOUNDED_FORWARD(arg));
}

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
inline constexpr auto reorder_transform = [](auto transform, auto && ... args) -> decltype(auto) {
	return ::bounded::detail::reorder_transform_implementation(
		bounded::constant<bounded::detail::normalize<sizeof...(args) - 1U>>,
		transform,
		BOUNDED_FORWARD(args)...
	);
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
		decltype(std::declval<Variant>().index())::max() + constant<1>
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




template<typename Function, std::size_t... indexes, typename... Variants> requires(sizeof...(indexes) == sizeof...(Variants))
constexpr decltype(auto) visit_implementation(
	Function && function,
	std::index_sequence<indexes...>,
	bounded::constant_t<0>,
	Variants && ... variants
) {
	return BOUNDED_FORWARD(function)(
		visitor_parameter<
			decltype(BOUNDED_FORWARD(variants)[bounded::constant<indexes>]),
			indexes
		>{BOUNDED_FORWARD(variants)[bounded::constant<indexes>]}...
	);
}

// This function accepts the pack of all variants twice. It passes over them
// once to get all the indexes, then again to pull out the values.
template<typename Function, std::size_t... indexes, typename Index, typename Variant, typename... Variants> requires(sizeof...(indexes) < sizeof...(Variants))
constexpr decltype(auto) visit_implementation(
	Function && function,
	std::index_sequence<indexes...> initial_indexes,
	Index possible_index,
	Variant const & variant,
	Variants && ... variants
) {
	auto found = [&]() -> decltype(auto) {
		return ::bounded::detail::visit_implementation(
			BOUNDED_FORWARD(function),
			std::index_sequence<indexes..., static_cast<std::size_t>(possible_index)>{},
			0_bi,
			BOUNDED_FORWARD(variants)...
		);
	};
	auto const search_index = variant.index();
	if constexpr (possible_index == search_index.max()) {
		return found();
	} else if (possible_index == search_index) {
		return found();
	} else {
		return ::bounded::detail::visit_implementation(
			BOUNDED_FORWARD(function),
			initial_indexes,
			possible_index + 1_bi,
			variant,
			BOUNDED_FORWARD(variants)...
		);
	}
}

inline constexpr auto visit_interface = [](auto transform) {
	return [=](auto && function, auto && ... variants) {
		return ::bounded::detail::visit_implementation(
			transform(BOUNDED_FORWARD(function)),
			std::index_sequence<>{},
			0_bi,
			variants...,
			BOUNDED_FORWARD(variants)...
		);
	};
};

} // namespace detail

inline constexpr struct visit_with_index_t {
private:
	static inline constexpr auto identity = [](auto && function) -> decltype(auto) {
		return BOUNDED_FORWARD(function);
	};
public:
	// Any number of variants (including 0) followed by one function
	template<typename... Args> requires detail::is_variants_then_visit_function<sizeof...(Args) - 1U, decltype(identity), Args...>
	constexpr decltype(auto) operator()(Args && ... args) const {
		return detail::reorder_transform(detail::visit_interface(identity), BOUNDED_FORWARD(args)...);
	}
} visit_with_index;

// Accepts any number of variants (including 0) followed by one function with
// arity equal to the number of variants
inline constexpr struct visit_t {
private:
	// The reference is safe in the inner lambda because this function lives
	// only for the duration of the call
	static inline constexpr auto get_value_only = [](auto && function) {
		return [&](auto && ... args) BOUNDED_NOEXCEPT_DECLTYPE(
			BOUNDED_FORWARD(function)(BOUNDED_FORWARD(args).value...)
		);
	};
public:
	template<typename... Args> requires detail::is_variants_then_visit_function<sizeof...(Args) - 1U, decltype(get_value_only), Args...>
	constexpr decltype(auto) operator()(Args && ... args) const {
		return detail::reorder_transform(detail::visit_interface(get_value_only), BOUNDED_FORWARD(args)...);
	}
} visit;

}	// namespace bounded
