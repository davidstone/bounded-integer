// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.format;

import bounded.bounded_integer;

import std_module;

template<bounded::bounded_integer T, typename CharT>
struct std::formatter<T, CharT> : private std::formatter<typename T::underlying_type, CharT> {
private:
	using base = std::formatter<typename T::underlying_type, CharT>;
public:
	using base::parse;

	template<typename FormatContext>
	constexpr auto format(bounded::bounded_integer auto const x, FormatContext & context) const {
		return base::format(x.value(), context);
	}
};
