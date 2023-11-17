// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.subtractable;

namespace containers {

export template<typename LHS, typename RHS = LHS>
concept subtractable = requires(LHS const lhs, RHS const rhs) { lhs - rhs; };

} // namespace containers
