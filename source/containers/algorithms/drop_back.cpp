// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.drop_back;

import containers.range_size_t;
import containers.size;
import containers.sized_range;
import containers.take;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace containers {

// TODO: Expand to bidirectional ranges
export template<sized_range Range, bounded::bounded_integer Count>
	requires(
		0_bi <= numeric_traits::min_value<Count> and
		numeric_traits::min_value<Count> <= numeric_traits::max_value<range_size_t<Range>>
	)
constexpr auto drop_back_exactly(Range && source, Count const count) {
	auto const source_size = containers::size(source);
	if (source_size < count) {
		throw std::runtime_error(
			"Tried to `drop_back_exactly` more elements than were in the source range"
		);
	}
	auto const amount_to_keep = source_size - count;
	return containers::take(
		OPERATORS_FORWARD(source),
		bounded::increase_min<0>(amount_to_keep)
	);
}

} // namespace containers
