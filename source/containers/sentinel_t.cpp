// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.sentinel_t;

import containers.begin_end;

import bounded;

namespace containers {

export template<typename Range>
using sentinel_t = decltype(containers::end(bounded::declval<Range>()));

} // namespace containers
