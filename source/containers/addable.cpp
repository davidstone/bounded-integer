// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.addable;

namespace containers {

export template<typename LHS, typename RHS>
concept addable = requires(LHS && lhs, RHS && rhs) {
	OPERATORS_FORWARD(lhs) + OPERATORS_FORWARD(rhs);
};

} // namespace containers
