// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.reserve_if_reservable;

import containers.size;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

export template<typename Container>
concept reservable = requires(Container & container, range_size_t<Container> size) { container.reserve(size); };

export template<typename Container>
constexpr void reserve_if_reservable(Container & container, range_size_t<Container> const size) {
	if constexpr (reservable<Container>) {
		container.reserve(size);
	}
}

} // namespace containers
