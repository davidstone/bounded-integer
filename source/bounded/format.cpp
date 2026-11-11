// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.format;

import bounded.bounded_integer;

import std_module;

template<bounded::bounded_integer T>
struct std::formatter<T> : private std::formatter<typename T::underlying_type> {
private:
	using base = std::formatter<typename T::underlying_type>;
public:
	using base::parse;

	constexpr auto format(bounded::bounded_integer auto const x, auto & context) const {
		return base::format(x.value(), context);
	}
};
