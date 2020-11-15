// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/append.hpp>
#include <containers/begin_end.hpp>
#include <containers/erase.hpp>
#include <containers/is_range.hpp>
#include <containers/range_view.hpp>
#include <containers/resizable_container.hpp>

#include <bounded/integer.hpp>

#include <initializer_list>

namespace containers {

constexpr void assign(resizable_container auto & destination, range auto && source) {
	auto it = begin(destination);
	auto first = begin(OPERATORS_FORWARD(source));
	auto last = end(OPERATORS_FORWARD(source));
	for (; first != last; ++first) {
		if (it == end(destination)) {
			break;
		}
		*it = *first;
		++it;
	}
	::containers::erase(destination, it, end(destination));
	::containers::append(destination, range_view(first, last));
}
template<resizable_container Container>
constexpr void assign(Container & destination, std::initializer_list<typename Container::value_type> init) {
	assign(destination, range_view(init));
}

} // namespace containers
