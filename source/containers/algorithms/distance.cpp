// Copyright David Stone 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.distance;

import containers.iter_difference_t;
import containers.iterator;
import containers.sentinel_for;

import bounded;
import std_module;

namespace containers {

using namespace bounded::literal;

export template<iterator InputIterator>
constexpr auto distance(InputIterator first, sentinel_for<InputIterator> auto const last) {
	if constexpr (requires { last - first; }) {
		return last - first;
	} else {
		auto difference = iter_difference_t<InputIterator>(0_bi);
		for (; first != last; ++first) {
			++difference;
		}
		return difference;
	}
}

} // namespace containers
