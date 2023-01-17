// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.common_iterator_functions;

import containers.is_iterator;

import bounded;
import operators;

export import containers.common_functions;

namespace containers {

export constexpr auto operator+(bounded::integral auto const offset, iterator auto const it) {
	return it + offset;
}

export using bounded::operator+=;
export using bounded::operator-=;
export using bounded::operator++;
export using bounded::operator--;
export using operators::operator++;
export using operators::operator--;

} // namespace containers
