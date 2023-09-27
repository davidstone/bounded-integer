// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.destroy;

import bounded.non_const;

import std_module;

namespace bounded {

export constexpr auto destroy = []<non_const T>(T & ref) noexcept(std::is_nothrow_destructible_v<T>) -> void {
	std::destroy_at(std::addressof(ref));
};

} // namespace bounded
