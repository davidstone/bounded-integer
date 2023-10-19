// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.range_reference_t;

import containers.iter_reference_t;
import containers.iterator_t;
import containers.range;

namespace containers {

export template<range Range>
using range_reference_t = iter_reference_t<iterator_t<Range>>;

} // namespace containers
