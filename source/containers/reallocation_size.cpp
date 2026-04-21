// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.reallocation_size;

import bounded;
import numeric_traits;

using namespace bounded::literal;

namespace containers {

// `minimum_acceptable_capacity` is usually the total number of elements in the
// container, but can also be a constant when that's known to be safe. If
// `minimum_acceptable_capacity` is at least `1`, the returned value will be
// greater than `current_capacity` by some constant factor and by at least `1`.
export template<typename Capacity>
constexpr auto reallocation_size(
	Capacity const current_capacity,
	bounded::bounded_integer auto const minimum_acceptable_capacity
) {
	return ::bounded::assume_in_range<Capacity>(bounded::max(
		minimum_acceptable_capacity,
		bounded::min(
			bounded::integer(current_capacity) * 2_bi,
			bounded::constant<numeric_traits::max_value<Capacity>>
		)
	));
}

} // namespace containers
