// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.size_then_use_range;

import containers.forward_range;
import containers.sized_range;

namespace containers {

// A range that you can get the size from and still access elements in the
// range. An unsized input range is an example of a range that does not model
// this concept.
export template<typename Range>
concept size_then_use_range = forward_range<Range const &> or sized_range<Range const &>;

} // namespace containers
