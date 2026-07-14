// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.mismatch;

import containers.begin_end;
import containers.iterator_t;
import containers.range;

import std_module;

namespace containers {

export template<range Range1, range Range2>
constexpr auto mismatch(
	Range1 && range1,
	Range2 && range2,
	auto const predicate
) -> std::pair<iterator_t<Range1>, iterator_t<Range2>> {
	auto const last1 = containers::end(range1);
	auto it1 = containers::begin(OPERATORS_FORWARD(range1));
	auto const last2 = containers::end(range2);
	auto it2 = containers::begin(OPERATORS_FORWARD(range2));
	while (it1 != last1 and it2 != last2 and predicate(*it1, *it2)) {
		++it1;
		++it2;
	}
	return std::pair(std::move(it1), std::move(it2));
}

export template<range Range1, range Range2>
constexpr auto mismatch(
	Range1 && range1,
	Range2 && range2
) -> std::pair<iterator_t<Range1>, iterator_t<Range2>> {
	return ::containers::mismatch(OPERATORS_FORWARD(range1), OPERATORS_FORWARD(range2), std::equal_to());
}

} // namespace containers
