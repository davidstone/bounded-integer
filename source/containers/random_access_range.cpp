// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.random_access_range;

import containers.bidirectional_range;
import containers.forward_random_access_range;

namespace containers {

export template<typename T>
concept random_access_range = bidirectional_range<T> and forward_random_access_range<T>;

} // namespace containers
