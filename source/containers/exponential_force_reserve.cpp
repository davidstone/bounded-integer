// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.exponential_force_reserve;

import containers.reallocation_size;

import bounded;

namespace containers {

// Guarantees an increase in reserved space
export template<typename Capacity>
constexpr auto exponential_force_reserve(
	auto & container,
	Capacity const current_capacity,
	bounded::bounded_integer auto const new_size
) -> void {
	static_assert(std::same_as<Capacity, decltype(container.capacity())>);
	BOUNDED_ASSERT(current_capacity >= container.capacity());
	container.reserve(bounded::check_in_range<Capacity>(::containers::reallocation_size(
		current_capacity,
		bounded::increase_min<1>(new_size)
	)));
}

} // namespace containers
