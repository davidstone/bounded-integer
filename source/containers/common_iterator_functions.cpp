// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.common_iterator_functions;

import containers.iterator;
import containers.iterator_addable;

import bounded;
import operators;
import std_module;

export import containers.common_functions;

namespace containers {

export template<bounded::integral LHS, iterator RHS> requires iterator_addable<RHS, LHS>
constexpr auto operator+(LHS const offset, RHS it) -> RHS {
	return std::move(it) + offset;
}

export using operators::operator+=;
export using operators::operator-=;
export using operators::operator++;
export using bounded::operator++;
export using operators::operator--;
export using bounded::operator--;

} // namespace containers
