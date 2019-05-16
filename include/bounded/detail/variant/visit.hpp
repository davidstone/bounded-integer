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

// I would put these as private static functions of a reorder_transform_t class,
// but I rely on argument dependent lookup to delay resolution of the function
// in the trailing return type. I prefix the name with `bounded_` to minimize
// the chances of accidental ADL
struct reorder_transform_tag{};

template<typename Function, typename TransformFunction, typename... Args>
constexpr auto bounded_reorder_transform_implementation(
	Function && function,
	TransformFunction && transform,
	reorder_transform_tag,
	Args && ... args
) BOUNDED_NOEXCEPT_DECLTYPE(
	BOUNDED_FORWARD(transform)(BOUNDED_FORWARD(function), BOUNDED_FORWARD(args)...)
)
template<typename Arg0, typename Arg1, typename Arg2, typename... Args, BOUNDED_REQUIRES(!std::is_same_v<Arg2, reorder_transform_tag>)>
constexpr auto bounded_reorder_transform_implementation(Arg0 && arg0, Arg1 && arg1, Arg2 && arg2, Args && ... args) BOUNDED_NOEXCEPT_DECLTYPE(
	bounded_reorder_transform_implementation(BOUNDED_FORWARD(arg1), BOUNDED_FORWARD(arg2), BOUNDED_FORWARD(args)..., BOUNDED_FORWARD(arg0))
)

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
auto reorder_transform = [](auto && ... args) BOUNDED_NOEXCEPT_DECLTYPE(
	::bounded::detail::bounded_reorder_transform_implementation(
		BOUNDED_FORWARD(args)...,
		reorder_transform_tag{}
	)
);

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


template<typename Function, typename Args, typename... Lists>
inline constexpr auto is_cartesian_product_callable_impl = false;

template<typename Function, typename... Args>
inline constexpr auto is_cartesian_product_callable_impl<Function, types<Args...>> = std::is_invocable_v<Function, Args...>;

template<typename Function, typename... Args, typename... Types, typename... Rest>
inline constexpr auto is_cartesian_product_callable_impl<Function, types<Args...>, types<Types...>, Rest...> = (
	... and
	is_cartesian_product_callable_impl<Function, types<Args..., Types>, Rest...>
);

// Requires that Lists is actually a variadic pack of types
template<typename Function, typename... Lists>
inline constexpr auto is_cartesian_product_callable = std::is_invocable_v<Function>;

template<typename Function, typename... Types, typename... Rest>
inline constexpr auto is_cartesian_product_callable<Function, types<Types...>, Rest...> = (
	... and
	is_cartesian_product_callable_impl<Function, types<Types>, Rest...>
);

template<typename Function, typename... Variants>
inline constexpr auto is_visitable = is_cartesian_product_callable<Function, variant_types<Variants>...>;



template<typename Function, std::size_t... indexes, typename... Variants, BOUNDED_REQUIRES(sizeof...(indexes) == sizeof...(Variants))>
constexpr auto visit_implementation(Function && function, std::index_sequence<indexes...>, bounded::constant_t<0>, Variants && ... variants) {
	return BOUNDED_FORWARD(function)(
		visitor_parameter<
			decltype(BOUNDED_FORWARD(variants)[bounded::constant<indexes>]),
			indexes
		>{BOUNDED_FORWARD(variants)[bounded::constant<indexes>]}...
	);
}

// This function accepts the pack of all variants twice. It passes over them
// once to get all the indexes, then again to pull out the values.
template<
	typename Function,
	std::size_t... indexes,
	typename Index,
	typename Variant,
	typename... Variants,
	BOUNDED_REQUIRES(sizeof...(indexes) < sizeof...(Variants))
>
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

} // namespace detail
	
constexpr struct visit_with_index_t {
private:
	static inline constexpr struct impl_t {
		template<typename Function, typename... Variants, BOUNDED_REQUIRES(detail::is_visitable<Function, Variants...>)>
		constexpr decltype(auto) operator()(Function && function, Variants && ... variants) const {
			return ::bounded::detail::visit_implementation(
				BOUNDED_FORWARD(function),
				std::index_sequence<>{},
				0_bi,
				variants...,
				BOUNDED_FORWARD(variants)...
			);
		}
	} impl{};
public:
	// Any number of variants (including 0) followed by one function
	template<typename... Args, BOUNDED_REQUIRES(sizeof...(Args) >= 1)>
	constexpr auto operator()(Args && ... args) const BOUNDED_NOEXCEPT_DECLTYPE(
		detail::reorder_transform(BOUNDED_FORWARD(args)..., impl)
	)
} visit_with_index;

// TODO: This will be simpler to implement when clang supports explicit template
// arguments on lambdas and / or requires clauses
// Accepts any number of variants (including 0) followed by one function with
// arity equal to the number of variants
constexpr struct visit_t {
private:
	template<typename Function>
	struct visit_function {
	private:
		Function && m_function;
	public:
		explicit constexpr visit_function(Function && function) noexcept:
			m_function(BOUNDED_FORWARD(function))
		{
		}
		template<typename... Values>
		constexpr auto operator()(Values && ... values) && BOUNDED_NOEXCEPT_DECLTYPE(
			BOUNDED_FORWARD(m_function)(BOUNDED_FORWARD(values).value...)
		)
	};
	static inline constexpr struct impl_t {
		template<typename Function, typename... Variants, BOUNDED_REQUIRES(detail::is_visitable<visit_function<Function>, Variants...>)>
		constexpr decltype(auto) operator()(Function && function, Variants && ... variants) const {
			return ::bounded::detail::visit_implementation(
				visit_function<Function>(BOUNDED_FORWARD(function)),
				std::index_sequence<>{},
				0_bi,
				variants...,
				BOUNDED_FORWARD(variants)...
			);
		}
	} impl{};
public:
	template<typename... Args, BOUNDED_REQUIRES(sizeof...(Args) >= 1)>
	constexpr auto operator()(Args && ... args) const BOUNDED_NOEXCEPT_DECLTYPE(
		detail::reorder_transform(BOUNDED_FORWARD(args)..., impl)
	)
} visit;

}	// namespace bounded
