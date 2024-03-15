// Copyright David Stone 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.character;

import std_module;

namespace bounded {

export template<typename T>
concept character =
	std::same_as<T, char> or
	std::same_as<T, wchar_t> or
	std::same_as<T, char8_t> or
	std::same_as<T, char16_t> or
	std::same_as<T, char32_t>;

} // namespace bounded
