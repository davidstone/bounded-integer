// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/comparison.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/unreachable.hpp>

namespace bounded {

constexpr auto assume_in_range(bounded_integer auto const value, bounded_integer auto const minimum, bounded_integer auto const maximum) {
	if (value < minimum or maximum < value) {
		bounded::unreachable();
	}
	return value;
}

} // namespace bounded
