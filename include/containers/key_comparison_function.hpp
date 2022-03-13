// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/ordered_associative_container.hpp>

#include <compare>

namespace containers {
namespace detail {

template<typename ExtractKey>
struct extract_key_to_compare {
	extract_key_to_compare() = default;
	explicit constexpr extract_key_to_compare(ExtractKey extract_key):
		m_extract_key(std::move(extract_key))
	{
	}

	constexpr auto operator()(auto const & lhs, auto const & rhs) const {
		return m_extract_key(lhs) <=> m_extract_key(rhs);
	}

private:
	[[no_unique_address]] ExtractKey m_extract_key;
};

} // namespace detail

template<ordered_associative_range Container>
constexpr auto key_comparison_function(Container const & container) {
	if constexpr (requires(Container const & c) { c.extract_key(); }) {
		return extract_key_to_compare(container.extract_key());
	} else {
		// TODO: ???
		assert(false);
	}
}

} // namespace containers
