// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.relocate_in_order;

import bounded;

namespace containers {

export constexpr auto relocate_in_order(auto it, auto & ... xs) {
	(..., bounded::relocate_at(*it++, xs));
}

} // namespace containers
