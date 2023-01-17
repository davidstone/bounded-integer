// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.algorithms.negate;

import std_module;

namespace containers {

export constexpr auto negate(auto pred) {
	return [predicate = std::move(pred)](auto && ... args) {
		return !predicate(OPERATORS_FORWARD(args)...);
	};
}

} // namespace containers
