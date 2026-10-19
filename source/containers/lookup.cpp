// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.lookup;

import containers.associative_container;
import containers.begin_end;
import containers.map_value_type;

import std_module;

namespace containers {

export template<map_range Map, typename Key = typename std::remove_reference_t<Map>::key_type>
constexpr auto lookup(Map && map, Key && key) {
	auto const it = map.find(OPERATORS_FORWARD(key));
	return it != containers::end(map) ?
		std::addressof(get_mapped(*it)) :
		nullptr;
}

export template<set_range Set, typename Key = typename std::remove_reference_t<Set>::key_type>
constexpr auto lookup(Set && set, Key && key) {
	auto const it = set.find(OPERATORS_FORWARD(key));
	return it != containers::end(set) ?
		std::addressof(*it) :
		nullptr;
}

export template<associative_range A, typename Key = typename std::remove_reference_t<A>::key_type>
constexpr auto lookup_value(A && a, Key && key) {
	auto const it = a.find(OPERATORS_FORWARD(key));
	return it != containers::end(a) ?
		std::addressof(*it) :
		nullptr;
}

} // namespace containers