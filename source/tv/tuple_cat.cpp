// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>
#include <operators/forward.hpp>

export module tv.tuple_cat;

import tv.tuple;
import tv.tuple_size;

import bounded;
import std_module;

namespace tv {

using namespace bounded::literal;

struct index_pairs {
	std::size_t outer;
	std::size_t inner;
};

template<std::size_t... inner_indexes>
constexpr auto add_pack(
	auto it,
	std::size_t const outer_index,
	std::index_sequence<inner_indexes...>
) {
	(..., (*it++ = {outer_index, inner_indexes}));
	return it;
}

constexpr auto make_indexes(auto const... sizes) {
	constexpr auto total = (0_bi + ... + sizes);
	auto result = std::array<index_pairs, total.value()>();
	auto it = result.begin();
	auto outer_index = 0UZ;
	(..., (it = add_pack(it, outer_index++, bounded::make_index_sequence(sizes))));
	BOUNDED_ASSERT(it == result.end());
	return result;
}

template<auto indexes, typename Tuples, std::size_t... index_indexes>
constexpr auto impl(
	Tuples tuples,
	std::index_sequence<index_indexes...>
) {
	return tuple<
		tuple_element<
			indexes[index_indexes].inner,
			std::remove_cvref_t<tuple_element<indexes[index_indexes].outer, Tuples>>
		>...
	>(
		std::move(tuples)[bounded::constant<indexes[index_indexes].outer>][bounded::constant<indexes[index_indexes].inner>]...
	);
}

export template<tuple_like... Tuples>
	requires(... and bounded::constructible_from<std::decay_t<Tuples>, Tuples &&>)
constexpr auto tuple_cat(Tuples && ... tuples) {
	constexpr auto indexes = make_indexes(tuple_size<Tuples>...);
	return impl<indexes>(
		tv::tie(OPERATORS_FORWARD(tuples)...),
		std::make_index_sequence<indexes.size()>()
	);
}

}	// namespace tv