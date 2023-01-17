// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.hash;

import bounded.integer;
import bounded.is_bounded_integer;

import std_module;

template<bounded::bounded_integer Integer>
struct std::hash<Integer> {
	using argument_type = Integer;
	static constexpr decltype(auto) operator()(argument_type const & argument) {
		using underlying_type = typename argument_type::underlying_type;
		return hash<underlying_type>{}(static_cast<underlying_type>(argument));
	}
};

using integer = bounded::integer<0, 0>;

static_assert(std::same_as<
	decltype(std::hash<integer>()(integer())),
	decltype(std::hash<integer::underlying_type>()(integer::underlying_type()))
>);