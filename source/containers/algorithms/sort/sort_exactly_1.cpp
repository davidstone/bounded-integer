// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.sort_exactly_1;

import bounded;

namespace containers {

export constexpr auto sort_exactly_n([[maybe_unused]] auto const it, bounded::constant_t<1>, [[maybe_unused]] auto const compare) -> void {
}

export constexpr auto sort_exactly_n_relocate(auto it, bounded::constant_t<1>, auto const out, [[maybe_unused]] auto const compare) {
	bounded::relocate_at(*out, *it);
	++it;
	return it;
}

} // namespace containers
