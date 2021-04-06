// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/is_range.hpp>

#include <bounded/detail/construct_destroy.hpp>

namespace containers {

constexpr void destroy_range(range auto && r) {
	for (auto & value : OPERATORS_FORWARD(r)) {
		bounded::destroy(value);
	}
}

} // namespace containers
