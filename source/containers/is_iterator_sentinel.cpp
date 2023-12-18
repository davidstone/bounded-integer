// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.is_iterator_sentinel;

import bounded;

export import containers.is_iterator;

namespace containers {

export template<typename Sentinel, typename Iterator>
concept sentinel_for = iterator<Iterator> and bounded::equality_comparable<Iterator, Sentinel>;

} // namespace containers
