// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/minmax.hpp>

namespace bounded {

constexpr auto clamp(bounded_integer auto const & value, bounded_integer auto const & minimum, bounded_integer auto const & maximum) {
	return min(
		max(
			value,
			minimum
		),
		maximum
	);
}

} // namespace bounded
