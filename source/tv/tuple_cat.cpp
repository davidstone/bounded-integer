// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module tv.tuple_cat;

import tv.tuple;
import tv.tuple_size;

import bounded;
import std_module;

namespace tv {

template<typename Indexes, typename Tuple>
struct indexed_tuple;

template<std::size_t... indexes, typename Tuple>
struct indexed_tuple<std::index_sequence<indexes...>, Tuple> {
	Tuple && tuple;
};

template<typename Tuple>
indexed_tuple(Tuple &&) -> indexed_tuple<std::make_index_sequence<tuple_size<Tuple>.value()>, Tuple>;

struct tuple_cat_t {
private:
	template<std::size_t... first_indexes, typename First, std::size_t... second_indexes, typename Second>
	static constexpr auto impl(
		indexed_tuple<std::index_sequence<first_indexes...>, First> first,
		indexed_tuple<std::index_sequence<second_indexes...>, Second> second,
		auto && ... tail
	) {
		return operator()(
			tuple<
				tuple_element<first_indexes, std::remove_cvref_t<First>>...,
				tuple_element<second_indexes, std::remove_cvref_t<Second>>...
			>(
				OPERATORS_FORWARD(first).tuple[bounded::constant<first_indexes>]...,
				OPERATORS_FORWARD(second).tuple[bounded::constant<second_indexes>]...
			),
			OPERATORS_FORWARD(tail).tuple...
		);
	}

public:
	template<tuple_like... Tuples> requires(... and bounded::constructible_from<std::decay_t<Tuples>, Tuples &&>)
	static constexpr auto operator()(Tuples && ... tuples) {
		if constexpr (sizeof...(tuples) == 0) {
			return tuple<>();
		} else if constexpr (sizeof...(tuples) == 1) {
			return (..., OPERATORS_FORWARD(tuples));
		} else {
			return impl(indexed_tuple{OPERATORS_FORWARD(tuples)}...);
		}
	}
};
export constexpr auto tuple_cat = tuple_cat_t();

}	// namespace tv