// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.bidirectional_range;

import containers.bidirectional_iterator;
import containers.forward_range;
import containers.iterator_t;

namespace containers {

export template<typename T>
concept bidirectional_range =
	forward_range<T> and
	bidirectional_iterator<iterator_t<T>>;

} // namespace containers
