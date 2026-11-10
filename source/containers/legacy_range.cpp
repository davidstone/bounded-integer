// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Adapter for functions that cannot be made compatible with a bounded::integer

module;

#include <operators/forward.hpp>

export module containers.legacy_range;

import containers.begin_end;
import containers.legacy_iterator;
import containers.size;
import containers.sized_range;
import containers.subrange;

import bounded;
import std_module;

namespace containers {

template<typename T>
using legacy_size = std::conditional_t<
	bounded::bounded_integer<T>,
	typename T::underlying_type,
	T
>;

export template<typename T>
constexpr auto legacy_range(T && r) {
	auto first = maybe_legacy_iterator(containers::begin(OPERATORS_FORWARD(r)));
	auto last = maybe_legacy_iterator(containers::end(OPERATORS_FORWARD(r)));
	if constexpr (sized_range<T>) {
		auto const size = containers::size(r);
		return containers::subrange(
			std::move(first),
			std::move(last),
			legacy_size<decltype(size)>(size)
		);
	} else {
		return containers::subrange(std::move(first), std::move(last));
	}
}

} // namespace containers
