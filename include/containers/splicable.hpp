// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/iterator_t.hpp>

namespace containers::detail {

// splice is required to be constant time
template<typename Container>
concept splicable = requires(Container target, iterator_t<Container const &> position, Container source) {
	target.splice(position, source);
};

} // namespace containers::detail
