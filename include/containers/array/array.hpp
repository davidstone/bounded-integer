// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/array/iterator.hpp>
#include <containers/common_container_functions.hpp>
#include <containers/is_container.hpp>
#include <containers/operator_bracket.hpp>

#include <bounded/integer.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
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

}	// namespace detail

template<typename T, std::size_t size, std::size_t... sizes>
struct array {
	using value_type = typename detail::array_value_type<T, sizes...>::type;

	using size_type = bounded::constant_t<size>;
	
	using const_iterator = detail::basic_array_iterator<value_type const, array>;
	using iterator = detail::basic_array_iterator<value_type, array>;

	CONTAINERS_OPERATOR_BRACKET_DEFINITIONS(array)

	// Consider this private. It must be public for the class to be an
	// aggregate
	value_type m_value[size];
};

namespace detail {

struct aggregate_initialization {
};

}	// namespace detail

template<typename T, std::size_t... sizes>
struct array<T, 0, sizes...> : detail::aggregate_initialization {
	using value_type = typename detail::array_value_type<T, sizes...>::type;

	using size_type = bounded::constant_t<0>;
	
	using const_iterator = detail::basic_array_iterator<value_type const, array>;
	// TODO: Should this be using iterator = const_iterator?
	using iterator = detail::basic_array_iterator<value_type, array>;
	
	// No operator[]
};

template<typename... Args>
array(Args && ...) -> array<std::common_type_t<std::decay_t<Args>...>, sizeof...(Args)>;

template<typename T, std::size_t... sizes>
constexpr auto begin(array<T, sizes...> const & container) noexcept {
	return typename array<T, sizes...>::const_iterator(container.m_value, detail::iterator_constructor);
}
template<typename T, std::size_t... sizes>
constexpr auto begin(array<T, sizes...> & container) noexcept {
	return typename array<T, sizes...>::iterator(container.m_value, detail::iterator_constructor);
}

template<typename T, std::size_t... sizes>
constexpr auto begin(array<T, 0, sizes...> const &) noexcept {
	return typename array<T, 0, sizes...>::const_iterator();
}
template<typename T, std::size_t... sizes>
constexpr auto begin(array<T, 0, sizes...> &) noexcept {
	return typename array<T, 0, sizes...>::iterator();
}

template<typename T, std::size_t size, std::size_t... sizes>
constexpr auto end(array<T, size, sizes...> const & container) noexcept {
	return begin(container) + bounded::constant<size>;
}
template<typename T, std::size_t size, std::size_t... sizes>
constexpr auto end(array<T, size, sizes...> & container) noexcept {
	return begin(container) + bounded::constant<size>;
}


template<typename T, std::size_t... sizes>
struct is_container_c<array<T, sizes...>> : std::true_type {};

}	// namespace containers
