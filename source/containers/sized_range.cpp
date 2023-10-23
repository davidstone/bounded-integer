// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.sized_range;

import containers.forward_random_access_range;
import containers.has_member_size;
import containers.range;

namespace containers {

export template<typename Range>
concept sized_range = (range<Range> and has_member_size<Range>) or forward_random_access_range<Range>;

} // namespace containers
