// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/no_unique_address.hpp>

#include <operators/returns.hpp>

export module containers.iterator_to_sentinel;

import std_module;

namespace containers {

export template<typename Iterator>
struct iterator_to_sentinel {
private:
	BOUNDED_NO_UNIQUE_ADDRESS Iterator m_base;
public:
	iterator_to_sentinel() = default;

	constexpr explicit iterator_to_sentinel(Iterator iterator):
		m_base(std::move(iterator))
	{
	}

	friend constexpr auto operator==(iterator_to_sentinel const &, iterator_to_sentinel const &) -> bool = default;
	friend constexpr auto operator==(
		iterator_to_sentinel const & lhs,
		auto const & rhs
	) OPERATORS_RETURNS(
		lhs.m_base == rhs
	)
};

} // namespace containers
