// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.iterator_t;

import containers.is_range;

namespace containers {

export template<range Range>
using iterator_t = iterator_t_impl<Range>;

} // namespace containers
