// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstddef>
#include <utility>

namespace containers {
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

}	// namespace detail
}	// namespace containers
