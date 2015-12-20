// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/common_container_functions.hpp>
#include <containers/array/iterator.hpp>

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

	using size_type = bounded::integer<size, size>;
	
	using const_iterator = basic_array_iterator<value_type const, array>;
	using iterator = basic_array_iterator<value_type, array>;

	constexpr auto data() const noexcept {
		return m_value;
	}
	constexpr auto data() noexcept {
		return m_value;
	}

	constexpr auto begin() const noexcept {
		return const_iterator(data());
	}
	constexpr auto begin() noexcept {
		return iterator(data());
	}

	constexpr auto end() const noexcept {
		return begin() + bounded::constant<size>;
	}
	constexpr auto end() noexcept {
		return begin() + bounded::constant<size>;
	}

	constexpr auto && operator[](index_type<array> const index) const noexcept {
		return *(begin() + index);
	}
	constexpr auto && operator[](index_type<array> const index) noexcept {
		return *(begin() + index);
	}


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
	using iterator = basic_array_iterator<value_type, array>;
	
	constexpr array() noexcept = default;

	// This class is technically not an aggregate because I defined this
	// constructor, but I do not think it changes observable behavior.
	//
	// This extra parameter is an aggregate and thus can be constructed with {}.
	// This constructor allows the syntax array<T, 0> = {{}}
	constexpr array(aggregate_initialization) noexcept {}

	constexpr auto data() const noexcept {
		return nullptr;
	}

	constexpr auto begin() const noexcept {
		return const_iterator(data());
	}

	constexpr auto end() const noexcept {
		return begin();
	}
	
	// No operator[]
};


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
