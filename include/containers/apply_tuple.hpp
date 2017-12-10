// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace containers {

// I should probably add a conditional noexcept specification to these
template<typename Function, typename Tuple, std::size_t... indexes>
constexpr decltype(auto) apply_helper(Function && f, Tuple && tuple_args, std::index_sequence<indexes...>) {
	return std::forward<Function>(f)(std::get<indexes>(std::forward<Tuple>(tuple_args))...);
}

template<typename Function, typename Tuple>
constexpr decltype(auto) apply(Function && f, Tuple && tuple_args) {
	using indexes = std::make_index_sequence<std::tuple_size<Tuple>::value>;
	return apply_helper(std::forward<Function>(f), std::forward<Tuple>(tuple_args), indexes{});
}

}	// namespace containers
