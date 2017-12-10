// Make an array with automatically deduced size and type
// Copyright (C) 2015 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <containers/array/array.hpp>

#include <bounded/integer.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

template<std::size_t number_of_values, std::size_t... dimensions>
struct final_dimension {
	static_assert(number_of_values % (dimensions * ...) == 0, "Incorrect number of parameters to deduce dimension.");
	static constexpr auto value = number_of_values / (dimensions * ...);
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
	return array<element_type, dimensions...>{ std::forward<Args>(args)... };
}
template<std::size_t... dimensions, typename... Args>
constexpr auto make_explicit_array(Args && ... args) noexcept {
	return array<std::common_type_t<std::decay_t<Args>...>, dimensions...>{ std::forward<Args>(args)... };
}


// These assume you did not specify the inner-most dimension.
template<typename element_type, std::size_t... dimensions, typename... Args>
constexpr auto make_array(Args && ... args) noexcept {
	return array<
		element_type,
		detail::final_dimension<sizeof...(Args), dimensions...>::value, dimensions...
	>{ std::forward<Args>(args)... };
}

template<std::size_t... dimensions, typename... Args>
constexpr auto make_array(Args && ... args) noexcept {
	return array<
		std::common_type_t<std::decay_t<Args>...>,
		detail::final_dimension<sizeof...(Args), dimensions...>::value, dimensions...
	>{ std::forward<Args>(args)... };
}


namespace detail {

// Current versions of compilers use O(n) template depth in make_index_sequence.
// This is a O(log(n)) depth implementation.

template<std::size_t, typename>
struct add_n_c;

template<std::size_t n, std::size_t... indexes>
struct add_n_c<n, std::index_sequence<indexes...>> {
	using type = std::index_sequence<(n + indexes)...>;
};

template<std::size_t n, typename Sequence>
using add_n = typename add_n_c<n, Sequence>::type;


template<typename LHS, typename RHS>
struct concatenate_c;

template<std::size_t... lhs, std::size_t... rhs>
struct concatenate_c<std::index_sequence<lhs...>, std::index_sequence<rhs...>> {
	using type = std::index_sequence<lhs..., rhs...>;
};

template<typename LHS, typename RHS>
using concatenate = typename concatenate_c<LHS, RHS>::type;


template<std::size_t size>
struct make_index_sequence_c;

template<std::size_t size>
using make_index_sequence = typename make_index_sequence_c<size>::type;

template<std::size_t size>
struct make_index_sequence_c {
	using type = concatenate<
		make_index_sequence<size / 2>,
		add_n<size / 2, make_index_sequence<size - (size / 2)>>
	>;
};

template<>
struct make_index_sequence_c<1> {
	using type = std::index_sequence<0>;
};
template<>
struct make_index_sequence_c<0> {
	using type = std::index_sequence<>;
};

// Use the comma operator to expand the variadic pack
// Move the last element in if possible. Order of evaluation is well-defined for
// aggregate initialization, so there is no risk of copy-after-move
template<std::intmax_t size, typename T, std::size_t... indexes>
constexpr auto make_array_n_impl(T && value, std::index_sequence<indexes...>) BOUNDED_NOEXCEPT_VALUE(
	array<std::decay_t<T>, size>{ (static_cast<void>(indexes), value)..., std::forward<T>(value) }
)

}	// namespace detail

template<typename T, typename overflow_policy, bounded::storage_type storage, bool poisoned>
constexpr auto make_array_n(bounded::constant_t<0, overflow_policy, storage, poisoned>, T &&) noexcept {
	return array<std::decay_t<T>, 0>{};
}

template<typename T, std::intmax_t size, typename overflow_policy, bounded::storage_type storage, bool poisoned>
constexpr auto make_array_n(bounded::constant_t<size, overflow_policy, storage, poisoned>, T && value) noexcept(std::is_nothrow_move_constructible<std::decay_t<T>>::value and std::is_nothrow_constructible<std::decay_t<T>, T &&>::value and (size == 1 or std::is_nothrow_copy_constructible<std::decay_t<T>>::value)) {
	return detail::make_array_n_impl<size>(std::forward<T>(value), detail::make_index_sequence<size - 1>{});
}

}	// namespace containers
