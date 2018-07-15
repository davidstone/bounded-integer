// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace containers {
namespace detail {

template<typename...>
struct types{};

template<typename T>
struct types<T> {
	using type = T;
};

template<typename... LHS, typename... RHS>
constexpr bool operator==(types<LHS...>, types<RHS...>) noexcept {
	return false;
}

template<typename... Ts>
constexpr bool operator==(types<Ts...>, types<Ts...>) noexcept {
	return true;
}

template<typename... LHS, typename... RHS>
constexpr bool operator!=(types<LHS...> lhs, types<RHS...> rhs) noexcept {
	return !(lhs == rhs);
}

}	// namespace detail

}	// namespace containers
