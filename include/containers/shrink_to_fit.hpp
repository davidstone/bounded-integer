// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/uninitialized.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

namespace containers {

using namespace bounded::literal;

template<range Container>
constexpr auto shrink_to_fit(Container & c) {
	auto const s = containers::size(c);
	if (s == c.capacity()) {
		return;
	}
	constexpr auto min_capacity = bounded::min_value<decltype(c.capacity())>;
	if constexpr (min_capacity > 0_bi) {
		if (c.capacity() == min_capacity) {
			return;
		}
	}
	auto temp = Container();
	temp.reserve(s);
	containers::uninitialized_relocate(c, containers::begin(temp));
	temp.append_from_capacity(s);
	c.append_from_capacity(-s);
	c = std::move(temp);
}

} // namespace containers
