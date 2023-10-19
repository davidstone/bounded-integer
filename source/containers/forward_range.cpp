// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.forward_range;

import containers.is_iterator;
import containers.iterator_t;
import containers.range;

namespace containers {

export template<typename T>
concept forward_range = range<T> and forward_iterator<iterator_t<T>>;

} // namespace containers
