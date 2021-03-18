// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/erase.hpp>
#include <containers/algorithms/keyed_binary_search.hpp>
#include <containers/associative_container.hpp>

namespace containers {

constexpr auto keyed_erase(associative_container auto & container, auto && key) {
	if constexpr (requires { container.find(OPERATORS_FORWARD(key)); }) {
		auto const it = container.find(OPERATORS_FORWARD(key));
		if (it == containers::end(container)) {
			return bounded::integer<0, 1>(bounded::constant<0>);
		}
		container.erase(it);
		return bounded::integer<0, 1>(bounded::constant<1>);
	} else {
		// TODO: This can be made more efficient for node-based containers
		auto const to_erase = keyed_equal_range(container, OPERATORS_FORWARD(key));
		auto const erased = containers::size(to_erase);
		containers::erase(container, containers::begin(to_erase), containers::end(to_erase));
		return erased;
	}
}

} // namespace containers
