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

constexpr auto add_pack(
	auto it,
	std::size_t const outer_index,
	auto const size
) {
	auto const [...inner_indexes] = bounded::index_sequence_struct(size);
	(..., (*it++ = {outer_index, inner_indexes.value()}));
	return it;
}

constexpr auto make_indexes(auto const... sizes) {
	constexpr auto total = (0_bi + ... + sizes);
	auto result = std::array<index_pairs, total.value()>();
	auto it = result.begin();
	auto outer_index = 0UZ;
	(..., (it = add_pack(it, outer_index++, sizes)));
	BOUNDED_ASSERT(it == result.end());
	return result;
}

// TODO: Simplify further when clang has constexpr structured bindings
export template<tuple_like... Tuples>
	requires(... and bounded::constructible_from<std::decay_t<Tuples>, Tuples &&>)
constexpr auto tuple_cat(Tuples && ... tuples) {
	constexpr auto indexes = ::tv::make_indexes(tuple_size<Tuples>...);
	auto const [...index_indexes] = bounded::index_sequence_struct<indexes.size()>();
	return tuple<
		tuple_element<
			indexes[index_indexes.value()].inner,
			std::remove_cvref_t<Tuples...[indexes[index_indexes.value()].outer]>
		>...
	>(
		OPERATORS_FORWARD(tuples...[indexes[index_indexes.value()].outer])[bounded::constant<indexes[index_indexes.value()].inner>]...
	);
}

}	// namespace tv