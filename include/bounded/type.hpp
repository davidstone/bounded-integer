// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace bounded {

template<typename...>
struct types {
};

template<typename T>
struct types<T> {
	using type = T;
};

template<typename... LHS, typename... RHS>
constexpr auto operator==(types<LHS...>, types<RHS...>) -> bool {
	return false;
}

template<typename... Ts>
constexpr auto operator==(types<Ts...>, types<Ts...>) -> bool {
	return true;
}

} // namespace bounded
