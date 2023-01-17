// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.reserve_if_reservable;

import containers.is_range;
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

export template<typename Capacity>
constexpr auto reallocation_size(Capacity const current_capacity, auto const current_size, auto const extra_elements) {
	return ::bounded::assume_in_range<Capacity>(bounded::max(
		bounded::integer(current_size) + bounded::integer(extra_elements),
		bounded::integer(current_capacity) * 2_bi
	));
}

export template<typename Range>
concept size_then_use_range = forward_range<Range const &> or requires(Range const & r) { containers::size(r); };

} // namespace containers
