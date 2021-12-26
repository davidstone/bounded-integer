// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/relocate.hpp>

#include <operators/forward.hpp>

#include <compare>
#include <utility>

namespace containers {

template<typename Key, typename Mapped>
struct map_value_type {
	using key_type = Key;
	using mapped_type = Mapped;

	friend auto operator<=>(map_value_type const & lhs, map_value_type const & rhs) = default;

	friend constexpr auto relocate(map_value_type & value) noexcept {
		return map_value_type{
			bounded::relocate(value.key),
			bounded::relocate(value.mapped)
		};
	}

	[[no_unique_address]] Key key;
	[[no_unique_address]] Mapped mapped;
};

namespace detail {

template<typename T>
concept key_mapped_pair = requires(T value) {
	value.key;
	value.mapped;
};

template<typename T>
concept first_second_pair = requires(T value) {
	value.first;
	value.second;
};

} // namespace detail

constexpr auto && get_key(detail::key_mapped_pair auto && value) {
	return OPERATORS_FORWARD(value).key;
}
constexpr auto && get_mapped(detail::key_mapped_pair auto && value) {
	return OPERATORS_FORWARD(value).mapped;
}

constexpr auto && get_key(detail::first_second_pair auto && value) {
	return OPERATORS_FORWARD(value).first;
}
constexpr auto && get_mapped(detail::first_second_pair auto && value) {
	return OPERATORS_FORWARD(value).second;
}

static_assert(detail::key_mapped_pair<map_value_type<int, int>>);

} // namespace containers
