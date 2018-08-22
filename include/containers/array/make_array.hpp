// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/array/array.hpp>
#include <containers/make_index_sequence.hpp>

#include <bounded/detail/forward.hpp>
#include <bounded/integer.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

template<std::size_t number_of_values, std::size_t... dimensions>
struct final_dimension {
	static_assert(number_of_values % (1 * ... * dimensions) == 0, "Incorrect number of parameters to deduce dimension.");
	static constexpr auto value = number_of_values / (1 * ... * dimensions);
};

}	// namespace detail

// If element_type is not specified, the type is deduced as common_type.
//
// This has some code duplication (rather than all versions forwarding to the
// explicitly typed overload of make_explicit_array) because every time you
// forward a large number of arguments, gcc and clang both use up a lot of
// memory. For creating an array of around 4000 elements, these functions were
// unusable when they did not construct the array directly.

// These assume that all of the dimensions have been passed in.
template<typename element_type, std::size_t... dimensions, typename... Args>
constexpr auto make_explicit_array(Args && ... args) noexcept {
	return array<element_type, dimensions...>{ BOUNDED_FORWARD(args)... };
}
template<std::size_t... dimensions, typename... Args>
constexpr auto make_explicit_array(Args && ... args) noexcept {
	return array<std::common_type_t<std::decay_t<Args>...>, dimensions...>{ BOUNDED_FORWARD(args)... };
}


// These assume you did not specify the inner-most dimension.
template<typename element_type, std::size_t... dimensions, typename... Args>
constexpr auto make_array(Args && ... args) noexcept {
	return array<
		element_type,
		detail::final_dimension<sizeof...(Args), dimensions...>::value, dimensions...
	>{ BOUNDED_FORWARD(args)... };
}

template<std::size_t... dimensions, typename... Args>
constexpr auto make_array(Args && ... args) noexcept {
	return array<
		std::common_type_t<std::decay_t<Args>...>,
		detail::final_dimension<sizeof...(Args), dimensions...>::value, dimensions...
	>{ BOUNDED_FORWARD(args)... };
}


namespace detail {

// Use the comma operator to expand the variadic pack
// Move the last element in if possible. Order of evaluation is well-defined for
// aggregate initialization, so there is no risk of copy-after-move
template<std::size_t size, typename T, std::size_t... indexes>
constexpr auto make_array_n_impl(T && value, std::index_sequence<indexes...>) BOUNDED_NOEXCEPT_VALUE(
	array<std::decay_t<T>, size>{ (static_cast<void>(indexes), value)..., BOUNDED_FORWARD(value) }
)

}	// namespace detail

template<auto size_, typename T>
constexpr auto make_array_n(bounded::constant_t<size_> size, T && value) noexcept(
	bounded::constant<size_> == 0_bi or
	(std::is_nothrow_move_constructible<std::decay_t<T>>{} and (bounded::constant<size_> == 1_bi or std::is_nothrow_copy_constructible<std::decay_t<T>>{}))
) {
	if constexpr (size == 0_bi) {
		return array<std::decay_t<T>, 0>{};
	} else {
		return detail::make_array_n_impl<size_>(BOUNDED_FORWARD(value), detail::make_index_sequence(size - 1_bi));
	}
}

}	// namespace containers
