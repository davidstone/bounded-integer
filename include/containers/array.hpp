// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/begin_end.hpp>
#include <containers/contiguous_iterator.hpp>
#include <containers/common_functions.hpp>

#include <bounded/integer.hpp>
#include <bounded/detail/make_index_sequence.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <operators/bracket.hpp>
#include <operators/forward.hpp>

#include <algorithm>
#include <cstddef>
#include <compare>
#include <iterator>
#include <span>
#include <type_traits>
#include <utility>

namespace containers {

template<typename T, std::size_t size, std::size_t... sizes>
struct array;

namespace detail {

template<typename T, std::size_t... sizes>
struct array_value_type {
	using type = array<T, sizes...>;
};
template<typename T>
struct array_value_type<T> {
	using type = T;
};

struct monostate {
	friend auto operator<=>(monostate, monostate) = default;
};

template<std::size_t size>
struct array_trait {
	template<typename T>
	using type = T[size];
};

template<>
struct array_trait<0> {
	template<typename>
	using type = monostate;
};

template<typename T, std::size_t size>
using array_type = typename array_trait<size>::template type<T>;

} // namespace detail

template<typename T, std::size_t size_, std::size_t... sizes>
struct array {
	static_assert(size_ <= static_cast<std::size_t>(numeric_traits::max_value<std::ptrdiff_t>));
	using value_type = typename detail::array_value_type<T, sizes...>::type;

	constexpr auto begin() const {
		using const_iterator = contiguous_iterator<value_type const, static_cast<std::ptrdiff_t>(size_)>;
		if constexpr (size() != 0) {
			return const_iterator(m_value);
		} else {
			return const_iterator();
		}
	}
	constexpr auto begin() {
		using iterator = contiguous_iterator<value_type, static_cast<std::ptrdiff_t>(size_)>;
		if constexpr (size() != 0) {
			return iterator(m_value);
		} else {
			return iterator();
		}
	}
	static constexpr auto size() {
		return bounded::constant<size_>;
	}

	OPERATORS_BRACKET_SEQUENCE_RANGE_DEFINITIONS

	friend auto operator<=>(array const &, array const &) = default;

	constexpr operator std::span<T const>() const & requires(sizeof...(sizes) == 0) {
		return std::span<T const>(m_value);
	}
	constexpr operator std::span<T>() & requires(sizeof...(sizes) == 0) {
		return std::span<T>(m_value);
	}
	constexpr operator std::span<T const, size_>() const & requires(sizeof...(sizes) == 0) {
		return std::span<T const, size_>(m_value);
	}
	constexpr operator std::span<T, size_>() & requires(sizeof...(sizes) == 0) {
		return std::span<T, size_>(m_value);
	}

	// Consider this private. It must be public for the class to be an
	// aggregate
	[[no_unique_address]] detail::array_type<value_type, size_> m_value;
};

template<typename... Args>
array(Args && ...) -> array<std::common_type_t<std::decay_t<Args>...>, sizeof...(Args)>;

template<typename T, std::size_t size>
array(c_array<T, size> &&) -> array<T, size>;


namespace detail {

// Use the comma operator to expand the variadic pack
// Move the last element in if possible. Order of evaluation is well-defined for
// aggregate initialization, so there is no risk of copy-after-move
template<std::size_t size, std::size_t... indexes>
constexpr auto make_array_n_impl(auto && value, std::index_sequence<indexes...>) {
	return array<std::decay_t<decltype(value)>, size>{(void(indexes), value)..., OPERATORS_FORWARD(value)};
}

} // namespace detail

template<auto size_>
constexpr auto make_array_n(bounded::constant_t<size_> size, auto && value) {
	if constexpr (size == bounded::constant<0>) {
		return array<std::decay_t<decltype(value)>, 0>{};
	} else {
		return detail::make_array_n_impl<size_>(OPERATORS_FORWARD(value), bounded::make_index_sequence(size - bounded::constant<1>));
	}
}


template<std::size_t index, typename T, std::size_t size>
constexpr auto && get(array<T, size> const & a) {
	return a[bounded::constant<index>];
}
template<std::size_t index, typename T, std::size_t size>
constexpr auto && get(array<T, size> & a) {
	return a[bounded::constant<index>];
}
template<std::size_t index, typename T, std::size_t size>
constexpr auto && get(array<T, size> && a) {
	return std::move(a[bounded::constant<index>]);
}

} // namespace containers

template<typename T, std::size_t size_>
struct std::tuple_size<::containers::array<T, size_>> : std::integral_constant<std::size_t, size_> {};

template<std::size_t index, typename T, std::size_t size>
struct std::tuple_element<index, ::containers::array<T, size>> {
	using type = T;
};
