// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/move_destroy_iterator.hpp>
#include <containers/begin_end.hpp>
#include <containers/is_range.hpp>
#include <containers/range_view.hpp>
#include <containers/size.hpp>

#include <bounded/integer.hpp>

namespace containers {

template<range Container>
constexpr auto shrink_to_fit(Container & c) {
	auto const s = containers::size(c);
	if (s == c.capacity() or c.capacity() == bounded::min_value<decltype(c.capacity())>) {
		return;
	}
	auto temp = Container(range_view(
		::containers::move_destroy_iterator(containers::begin(c)),
		::containers::move_destroy_iterator(containers::end(c))
	));
	c.append_from_capacity(-s);
	c = std::move(temp);
}

} // namespace containers
