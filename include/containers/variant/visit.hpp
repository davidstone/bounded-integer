// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/tuple.hpp>
#include <containers/type.hpp>

#include <cassert>
#include <type_traits>

namespace containers {
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
		std::move(args)[bounded::constant<sizeof...(Args)> - 1_bi](
			std::move(args)[bounded::constant<sizeof...(Args)> - 2_bi],
			std::move(args)[bounded::constant<indexes>]...
		)
	)
public:
	template<typename... Args>
	constexpr auto operator()(Args && ... args) const BOUNDED_NOEXCEPT_DECLTYPE(
		reorder_transform_t::implementation(
			tie(std::forward<Args>(args)...),
			make_index_sequence(bounded::constant<sizeof...(Args)> - 2_bi)
		)
	)
} reorder_transform;

}	// namespace detail

template<typename T, auto n>
struct visitor_parameter {
	static_assert(std::is_reference_v<T>);
	T value;
	bounded::constant_t<n> index;
};

template<typename T, auto n>
visitor_parameter(T &&, bounded::constant_t<n>) -> visitor_parameter<T &&, n>;

constexpr struct visitor_with_index {
private:
	template<typename Function, typename... Values>
	static constexpr decltype(auto) implementation(Function && function, bounded::constant_t<0>, tuple<Values...> values) {
		return apply(std::forward<Function>(function), std::move(values));
	}

	template<typename Function, typename Index, typename... Values, typename Variant, typename... Variants>
	static constexpr decltype(auto) implementation(Function && function, Index const current_index, tuple<Values...> values, Variant && variant, Variants && ... variants) noexcept(false)
	{
		auto found = [&]() -> decltype(auto) {
			return visitor_with_index::implementation(
				std::forward<Function>(function),
				0_bi,
				tuple_cat(
					std::move(values),
					tuple(visitor_parameter{std::forward<Variant>(variant)[current_index], current_index})
				),
				std::forward<Variants>(variants)...
			);
		};
		auto const search_index = variant.index();
		if constexpr (current_index == search_index.max()) {
			return found();
		} else if (current_index == search_index) {
			return found();
		} else {
			return implementation(
				std::forward<Function>(function),
				current_index + 1_bi,
				std::move(values),
				std::forward<Variant>(variant),
				std::forward<Variants>(variants)...
			);
		}
	}
	
public:
	// Any number of variants (including 0) followed by one function
	template<typename... Args, BOUNDED_REQUIRES(sizeof...(Args) >= 1)>
	constexpr decltype(auto) operator()(Args && ... args) const {
		return detail::reorder_transform(
			std::forward<Args>(args)...,
			[](auto && function, auto && ... variants) BOUNDED_NOEXCEPT_DECLTYPE(
				implementation(
					std::forward<decltype(function)>(function),
					0_bi,
					tuple{},
					std::forward<decltype(variants)>(variants)...
				)
			)
		);
	}
} visit_with_index;

// Accepts any number of variants (including 0) followed by one function with
// arity equal to the number of variants
constexpr auto visit = [](auto && ... args) -> decltype(auto) {
	return detail::reorder_transform(
		std::forward<decltype(args)>(args)...,
		[](auto && function, auto && ... variants) {
			return visit_with_index(
				std::forward<decltype(variants)>(variants)...,
				[&](auto && ... values) {
					return std::forward<decltype(function)>(function)(std::forward<decltype(values)>(values).value...);
				}
			);
		}
	);
};

}	// namespace containers
