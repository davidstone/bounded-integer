// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module tv.transform;

import tv.tuple;

import bounded;
import std_module;

using namespace bounded::literal;

namespace tv {

template<typename... Tuples>
constexpr auto all_tuple_sizes_equal = true;

template<typename Tuple, typename... Tuples>
constexpr auto all_tuple_sizes_equal<Tuple, Tuples...> = (... and (tuple_size<Tuple> == tuple_size<Tuples>));

template<typename... Tuples>
constexpr auto all_tuple_sizes = 0_bi;

template<typename Tuple, typename... Tuples>
constexpr auto all_tuple_sizes<Tuple, Tuples...> = tuple_size<Tuple>;

export template<tuple_like... Tuples>
constexpr auto transform(auto && function, Tuples && ... tuples) requires(all_tuple_sizes_equal<Tuples...>) {
	constexpr auto size = all_tuple_sizes<Tuples...>;
	auto single = [&](auto const index) -> decltype(auto) {
		return function(OPERATORS_FORWARD(tuples)[index]...);
	};
	auto all = [&]<std::size_t... indexes>(std::index_sequence<indexes...>) {
		return tuple<decltype(single(bounded::constant<indexes>))...>(single(bounded::constant<indexes>)...);
	};
	return all(std::make_index_sequence<static_cast<std::size_t>(size)>());
}

}	// namespace tv
