// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/array/iterator.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/operator_bracket.hpp>

#include <bounded/integer.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

namespace containers {
namespace detail {

template<typename T, std::size_t size>
struct array {
	using value_type = T;

	using size_type = bounded::constant_t<size>;
	
	using const_iterator = basic_array_iterator<value_type const, array>;
	using iterator = basic_array_iterator<value_type, array>;

	CONTAINERS_OPERATOR_BRACKET_DEFINITIONS

	// Consider this private. It must be public for the class to be an
	// aggregate
	value_type m_value[size];
};

struct aggregate_initialization {
};

template<typename T>
struct array<T, 0> {
	using value_type = T;

	using size_type = bounded::constant_t<0>;
	
	using const_iterator = basic_array_iterator<value_type const, array>;
	// TODO: Should this be using iterator = const_iterator?
	using iterator = basic_array_iterator<value_type, array>;
	
	constexpr array() noexcept = default;

	// This class is technically not an aggregate because I defined this
	// constructor, but I do not think it changes observable behavior.
	//
	// This extra parameter is an aggregate and thus can be constructed with {}.
	// This constructor allows the syntax array<T, 0> = {{}}
	constexpr array(aggregate_initialization) noexcept {}

	// No operator[]
};

template<typename T, std::size_t size>
constexpr auto begin(array<T, size> const & container) noexcept {
	return typename array<T, size>::const_iterator(container.m_value, detail::iterator_constructor);
}
template<typename T, std::size_t size>
constexpr auto begin(array<T, size> & container) noexcept {
	return typename array<T, size>::iterator(container.m_value, detail::iterator_constructor);
}

template<typename T>
constexpr auto begin(array<T, 0> const &) noexcept {
	return typename array<T, 0>::const_iterator();
}
template<typename T>
constexpr auto begin(array<T, 0> &) noexcept {
	return typename array<T, 0>::iterator();
}

template<typename T, std::size_t size>
constexpr auto end(array<T, size> const & container) noexcept {
	return begin(container) + bounded::constant<size>;
}
template<typename T, std::size_t size>
constexpr auto end(array<T, size> & container) noexcept {
	return begin(container) + bounded::constant<size>;
}


template<typename T, std::size_t dimension, std::size_t... dimensions>
struct multi_dimensional_array {
	using type = array<typename multi_dimensional_array<T, dimensions...>::type, dimension>;
};

template<typename T, std::size_t dimension>
struct multi_dimensional_array<T, dimension> {
	using type = array<T, dimension>;
};

}	// namespace detail

template<typename T, std::size_t... dimensions>
using array = typename detail::multi_dimensional_array<T, dimensions...>::type;

}	// namespace containers
