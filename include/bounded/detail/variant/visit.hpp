// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/tuple.hpp>
#include <bounded/detail/type.hpp>

#include <cassert>
#include <type_traits>

namespace bounded {
namespace detail {

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
constexpr struct reorder_transform_t {
private:
	template<typename... Args, auto... indexes>
	static constexpr auto implementation(tuple<Args...> args, std::index_sequence<indexes...>) BOUNDED_NOEXCEPT_DECLTYPE(
		std::move(args)[constant<sizeof...(Args)> - 1_bi](
			std::move(args)[constant<sizeof...(Args)> - 2_bi],
			std::move(args)[constant<indexes>]...
		)
	)
public:
	template<typename... Args>
	constexpr auto operator()(Args && ... args) const BOUNDED_NOEXCEPT_DECLTYPE(
		reorder_transform_t::implementation(
			tie(BOUNDED_FORWARD(args)...),
			make_index_sequence(constant<sizeof...(Args)> - 2_bi)
		)
	)
} reorder_transform;

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




template<typename Function, typename... Values>
constexpr auto visit_implementation(Function && function, constant_t<0>, tuple<Values...> values) BOUNDED_NOEXCEPT_DECLTYPE(
	apply(std::move(values), BOUNDED_FORWARD(function))
)

// TODO: noexcept?
template<typename Function, typename Index, typename... Values, typename Variant, typename... Variants>
constexpr decltype(auto) visit_implementation(Function && function, Index const current_index, tuple<Values...> values, Variant && variant, Variants && ... variants) {
	auto found = [&]() BOUNDED_NOEXCEPT_DECLTYPE(
		detail::visit_implementation(
			BOUNDED_FORWARD(function),
			0_bi,
			tuple_cat(
				std::move(values),
				tuple(
					visitor_parameter<
						decltype(BOUNDED_FORWARD(variant)[current_index]),
						current_index.value()
					>{BOUNDED_FORWARD(variant)[current_index]}
				)
			),
			BOUNDED_FORWARD(variants)...
		)
	);
	auto const search_index = variant.index();
	if constexpr (current_index == search_index.max()) {
		return found();
	} else if (current_index == search_index) {
		return found();
	} else {
		return detail::visit_implementation(
			BOUNDED_FORWARD(function),
			current_index + 1_bi,
			std::move(values),
			BOUNDED_FORWARD(variant),
			BOUNDED_FORWARD(variants)...
		);
	}
}

} // namespace detail
	
constexpr struct visit_with_index_t {
private:
	static constexpr struct impl_t {
		template<typename Function, typename... Variants, BOUNDED_REQUIRES(detail::is_visitable<Function, Variants...>)>
		constexpr auto operator()(Function && function, Variants && ... variants) const BOUNDED_NOEXCEPT_DECLTYPE(
			detail::visit_implementation(
				BOUNDED_FORWARD(function),
				0_bi,
				tuple{},
				BOUNDED_FORWARD(variants)...
			)
		)
	} impl;
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
	static constexpr struct impl_t {
		template<typename Function, typename... Variants, BOUNDED_REQUIRES(detail::is_visitable<visit_function<Function>, Variants...>)>
		constexpr auto operator()(Function && function, Variants && ... variants) const BOUNDED_NOEXCEPT_DECLTYPE(
			detail::visit_implementation(
				visit_function<Function>(BOUNDED_FORWARD(function)),
				0_bi,
				tuple{},
				BOUNDED_FORWARD(variants)...
			)
		)
	} impl;
public:
	template<typename... Args, BOUNDED_REQUIRES(sizeof...(Args) >= 1)>
	constexpr auto operator()(Args && ... args) const BOUNDED_NOEXCEPT_DECLTYPE(
		detail::reorder_transform(BOUNDED_FORWARD(args)..., impl)
	)
} visit;

}	// namespace bounded
