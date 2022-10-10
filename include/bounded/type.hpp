// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace bounded {

template<typename T>
struct type_t {
	using type = T;

	friend constexpr auto operator==(type_t, type_t) -> bool = default;
	template<typename U>
	friend constexpr auto operator==(type_t, type_t<U>) -> bool {
		return false;
	}
};

template<typename T>
inline constexpr auto type = type_t<T>();

} // namespace bounded
