// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.random_access_sentinel_for;

import containers.is_iterator_sentinel;
import containers.subtractable;

namespace containers {

export template<typename Sentinel, typename Iterator>
concept random_access_sentinel_for =
	sentinel_for<Sentinel, Iterator> and
	subtractable<Sentinel, Iterator>;

} // namespace containers
