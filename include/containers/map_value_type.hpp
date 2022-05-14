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

template<typename Key, typename Mapped>
map_value_type(Key, Mapped) -> map_value_type<Key, Mapped>;

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

struct get_key_t {
	constexpr auto operator()(detail::key_mapped_pair auto && value) const -> auto && {
		return OPERATORS_FORWARD(value).key;
	}
	constexpr auto operator()(detail::first_second_pair auto && value) const -> auto && {
		return OPERATORS_FORWARD(value).first;
	}
} inline constexpr get_key;

struct get_mapped_t {
	constexpr auto operator()(detail::key_mapped_pair auto && value) const -> auto && {
		return OPERATORS_FORWARD(value).mapped;
	}
	constexpr auto operator()(detail::first_second_pair auto && value) const -> auto && {
		return OPERATORS_FORWARD(value).second;
	}
} inline constexpr get_mapped;

} // namespace containers
