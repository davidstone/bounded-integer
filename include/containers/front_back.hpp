// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/advance.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_empty.hpp>
#include <containers/is_range.hpp>

#include <operators/forward.hpp>
#include <bounded/assert.hpp>
#include <bounded/integer.hpp>

#include <utility>

namespace containers {

constexpr decltype(auto) front(range auto && r) {
	BOUNDED_ASSERT(!containers::is_empty(r));
	return *containers::begin(OPERATORS_FORWARD(r));
}
constexpr decltype(auto) back(range auto && r) {
	BOUNDED_ASSERT(!containers::is_empty(r));
	return *containers::prev(containers::end(OPERATORS_FORWARD(r)));
}

} // namespace containers
