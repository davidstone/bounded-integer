// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/iterator_t.hpp>

namespace containers {

template<typename Container>
concept splicable = requires(Container target, iterator_t<Container const &> position, Container source) {
	target.splice(position, source);
};


template<typename Container>
concept constant_time_erasable = splicable<Container>;

} // namespace containers
