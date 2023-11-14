// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.is_empty;

import containers.begin_end;
import containers.range;
import containers.range_size_t;

import bounded;
import numeric_traits;
import std_module;

using namespace bounded::literal;

namespace containers {

template<typename Range>
concept never_empty_range = numeric_traits::min_value<range_size_t<Range>> > 0_bi;

export constexpr auto is_empty = [](range auto && r) {
	// The never_empty check is not needed for correctness, but allows this
	// function to be constexpr in more situations.
	return never_empty_range<decltype(r)> ? false : containers::begin(OPERATORS_FORWARD(r)) == containers::end(OPERATORS_FORWARD(r));
};

} // namespace containers
