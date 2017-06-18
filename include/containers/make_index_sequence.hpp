// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

#include <cstddef>
#include <utility>

namespace containers {
namespace detail {

using namespace bounded::literal;

// Current versions of compilers use O(n) template depth in make_index_sequence.
// This is a O(log(n)) depth implementation.

template<std::intmax_t n, std::size_t... indexes>
constexpr auto add_n(bounded::constant_t<n>, std::index_sequence<indexes...>) noexcept {
	return std::index_sequence<(n + indexes)...>{};
};


template<std::size_t... lhs, std::size_t... rhs>
constexpr auto concatenate(std::index_sequence<lhs...>, std::index_sequence<rhs...>) noexcept {
	return std::index_sequence<lhs..., rhs...>{};
};


template<std::size_t size>
struct make_index_sequence_c;

template<std::intmax_t size>
using make_index_sequence_t = typename make_index_sequence_c<size>::type;

template<>
struct make_index_sequence_c<0> {
	using type = std::index_sequence<>;
};
template<>
struct make_index_sequence_c<1> {
	using type = std::index_sequence<0>;
};
template<std::size_t size>
struct make_index_sequence_c {
	using type = decltype(concatenate(
		make_index_sequence_t<size / 2>{},
		add_n(bounded::constant<size> / 2_bi, make_index_sequence_t<size - (size / 2)>{})
	));
};

template<std::intmax_t n>
constexpr auto make_index_sequence(bounded::constant_t<n>) noexcept {
	return make_index_sequence_t<n>{};
}

}	// namespace detail
}	// namespace containers
