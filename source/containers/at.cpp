// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.at;

import containers.array;
import containers.index_type;
import containers.is_range;
import containers.size;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

export constexpr decltype(auto) at(range auto && r, auto const index) {
	auto const checked_index = bounded::check_in_range<std::out_of_range>(
		bounded::integer(index),
		0_bi,
		bounded::integer(containers::size(r)) - 1_bi
	);
	return OPERATORS_FORWARD(r)[static_cast<index_type<decltype(r)>>(checked_index)];
}

export constexpr decltype(auto) at(range auto && r, auto const index, bounded::unchecked_t) {
	using index_t = index_type<decltype(r)>;
	auto const converted = index_t(index);
	return OPERATORS_FORWARD(r)[converted];
}

} // namespace containers

static_assert(containers::at(containers::array({1, 2, 3}), 0_bi) == 1);
