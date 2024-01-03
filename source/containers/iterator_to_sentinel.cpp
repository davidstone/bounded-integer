// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.iterator_to_sentinel;

import bounded;
import std_module;

namespace containers {

export template<typename Iterator>
struct iterator_to_sentinel {
	iterator_to_sentinel() = default;

	constexpr explicit iterator_to_sentinel(Iterator iterator):
		m_base(std::move(iterator))
	{
	}

	friend constexpr auto operator==(
		bounded::equality_comparable<Iterator> auto const & lhs,
		iterator_to_sentinel const & rhs
	) -> bool {
		return lhs == rhs.m_base;
	}

private:
	[[no_unique_address]] Iterator m_base;
};

} // namespace containers
