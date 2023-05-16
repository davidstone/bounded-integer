// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.range_value_t;

import containers.is_range;
import containers.iter_value_t;
import containers.iterator_t;

namespace containers {

export template<range Range>
using range_value_t = iter_value_t<iterator_t<Range>>;

} // namespace containers
