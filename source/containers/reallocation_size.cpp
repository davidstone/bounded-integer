// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.reallocation_size;

import bounded;
import numeric_traits;

using namespace bounded::literal;

namespace containers {

export template<typename Capacity>
constexpr auto reallocation_size(Capacity const current_capacity, auto const current_size, auto const extra_elements) {
	return ::bounded::assume_in_range<Capacity>(bounded::max(
		bounded::integer(current_size) + bounded::integer(extra_elements),
		bounded::min(
			bounded::integer(current_capacity) * 2_bi,
			numeric_traits::max_value<Capacity>
		)
	));
}

} // namespace containers
