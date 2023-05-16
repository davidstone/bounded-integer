// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.key_comparison_function;

import containers.ordered_associative_container;

import std_module;

namespace containers {

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

template<typename>
constexpr auto always_false = false;

export template<ordered_associative_range Map>
constexpr auto key_comparison_function(Map const & map) {
	if constexpr (requires(Map const & c) { c.extract_key(); }) {
		return extract_key_to_compare(map.extract_key());
	} else {
		static_assert(always_false<Map>, "Unimplemented");
	}
}

} // namespace containers
