// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/erase.hpp>
#include <containers/append.hpp>
#include <containers/begin_end.hpp>
#include <containers/c_array.hpp>
#include <containers/is_range.hpp>
#include <containers/range_value_t.hpp>
#include <containers/range_view.hpp>
#include <containers/resizable_container.hpp>

#include <bounded/integer.hpp>

namespace containers {

constexpr void assign(resizable_container auto & destination, range auto && source) {
	auto it = containers::begin(destination);
	auto first = containers::begin(OPERATORS_FORWARD(source));
	auto last = containers::end(OPERATORS_FORWARD(source));
	for (; first != last; ++first) {
		if (it == containers::end(destination)) {
			break;
		}
		*it = *first;
		++it;
	}
	::containers::erase(destination, it, containers::end(destination));
	::containers::append(destination, range_view(first, last));
}
template<resizable_container Container, std::size_t init_size>
constexpr void assign(Container & destination, c_array<range_value_t<Container>, init_size> && init) {
	assign(destination, range_view(std::move(init)));
}

} // namespace containers
