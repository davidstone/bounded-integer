// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module containers.algorithms.splice;

import containers.algorithms.advance;
import containers.begin_end;
import containers.iterator_t;
import containers.splicable;

namespace containers {

// Not really happy with this interface yet
export template<splicable Container>
constexpr auto splice(
	Container & container,
	iterator_t<Container const &> const position,
	Container & other,
	iterator_t<Container const &> const first,
	iterator_t<Container const &> const last
) -> void {
	container.splice(position, other, first, last);
}

export template<splicable Container>
constexpr auto splice(
	Container & container,
	iterator_t<Container const &> const position,
	Container & other
) -> void {
	if constexpr (requires { container.splice(position, other); }) {
		container.splice(position, other);
	} else {
		container.splice(position, other, other.begin(), other.end());
	}
}

export template<splicable Container>
constexpr auto splice(
	Container & container,
	iterator_t<Container const &> const position,
	Container && other
) -> void {
	::containers::splice(container, position, other);
}

export template<splicable Container>
constexpr auto splice(
	Container & container,
	iterator_t<Container const &> const position,
	Container & other,
	iterator_t<Container const &> const it
) -> void {
	BOUNDED_ASSERT(it != containers::end(other));
	if constexpr (requires { container.splice(position, other, it); }) {
		container.splice(position, other, it);
	} else {
		::containers::splice(container, position, other, it, containers::next(it));
	}
}

} // namespace containers
