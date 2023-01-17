// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.repeat_n;

import containers.algorithms.compare;
import containers.algorithms.generate;
import containers.array;

import bounded;

namespace containers {

export template<typename Size, typename T>
constexpr auto repeat_n(Size const size, T && value) {
	return generate_n(size, bounded::value_to_function(OPERATORS_FORWARD(value)));
}

export template<typename T, typename Size>
constexpr auto repeat_default_n(Size const size) {
	return generate_n(size, bounded::construct<T>);
}

} // namespace containers

using namespace bounded::literal;

static_assert(containers::equal(containers::repeat_default_n<int>(0_bi), containers::array<int, 0_bi>{}));
static_assert(containers::equal(containers::repeat_default_n<int>(5_bi), containers::array{0, 0, 0, 0, 0}));
static_assert(containers::equal(containers::repeat_n(0_bi, 3), containers::array<int, 0_bi>{}));
static_assert(containers::equal(containers::repeat_n(2_bi, 3), containers::array{3, 3}));