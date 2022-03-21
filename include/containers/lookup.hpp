// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/associative_container.hpp>
#include <containers/begin_end.hpp>
#include <containers/map_value_type.hpp>

#include <operators/forward.hpp>

#include <memory>

namespace containers {

template<associative_range Map, typename Key = typename std::remove_reference_t<Map>::key_type>
constexpr auto lookup(Map && map, Key && key) {
	auto const it = map.find(OPERATORS_FORWARD(key));
	return it != containers::end(map) ?
		std::addressof(get_mapped(*it)) :
		nullptr;
}

} // namespace containers
