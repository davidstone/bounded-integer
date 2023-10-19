// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.count_type;

import containers.iterator_t;
import containers.offset_type;
import containers.range;

namespace containers {

export template<range Range>
using count_type = offset_type<iterator_t<Range &>>;

} // namespace containers
