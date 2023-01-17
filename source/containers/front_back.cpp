// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>
#include <bounded/assert.hpp>

export module containers.front_back;

import containers.algorithms.advance;
import containers.begin_end;
import containers.c_array;
import containers.is_empty;
import containers.is_range;

import std_module;

namespace containers {

export constexpr decltype(auto) front(range auto && r) {
	BOUNDED_ASSERT(!containers::is_empty(r));
	return *containers::begin(OPERATORS_FORWARD(r));
}
export constexpr decltype(auto) back(range auto && r) {
	BOUNDED_ASSERT(!containers::is_empty(r));
	return *containers::prev(containers::end(OPERATORS_FORWARD(r)));
}

} // namespace containers

constexpr containers::c_array<int, 1> one = {5};
static_assert(containers::front(one) == 5);
static_assert(containers::back(one) == 5);

constexpr containers::c_array<int, 3> three = {2, 3, 4};
static_assert(containers::front(three) == 2);
static_assert(containers::back(three) == 4);