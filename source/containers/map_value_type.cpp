// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.map_value_type;

import bounded;
import std_module;

namespace containers {

export template<typename Key, typename Mapped>
struct map_value_type {
	using key_type = Key;
	using mapped_type = Mapped;

	friend auto operator<=>(map_value_type const & lhs, map_value_type const & rhs) = default;

	friend constexpr auto relocate(map_value_type & value) noexcept -> map_value_type {
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

struct get_key_t {
	static constexpr auto operator()(key_mapped_pair auto && value) -> auto && {
		return OPERATORS_FORWARD(value).key;
	}
	static constexpr auto operator()(first_second_pair auto && value) -> auto && {
		return OPERATORS_FORWARD(value).first;
	}
};
export constexpr auto get_key = get_key_t();

struct get_mapped_t {
	static constexpr auto operator()(key_mapped_pair auto && value) -> auto && {
		return OPERATORS_FORWARD(value).mapped;
	}
	static constexpr auto operator()(first_second_pair auto && value) -> auto && {
		return OPERATORS_FORWARD(value).second;
	}
};
export constexpr auto get_mapped = get_mapped_t();

} // namespace containers

template<typename Key, typename Mapped>
struct std::formatter<containers::map_value_type<Key, Mapped>> {
	constexpr formatter() {
		m_underlying.set_brackets({}, {});
		m_underlying.set_separator(": ");
	}

	constexpr auto parse(std::format_parse_context & context) {
		return m_underlying.parse(context);
	}

	constexpr auto format(
		containers::map_value_type<Key, Mapped> const & value,
		auto & context
	) const {
		return m_underlying.format(
			std::pair<Key const &, Mapped const &>(value.key, value.mapped),
			context
		);
	}

private:
	std::formatter<std::pair<Key const &, Mapped const &>> m_underlying;
};
