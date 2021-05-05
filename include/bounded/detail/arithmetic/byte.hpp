// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstddef>

#include <bounded/detail/class.hpp>
#include <bounded/detail/is_bounded_integer.hpp>

#include <numeric_traits/min_max_value.hpp>

namespace bounded {

// TODO: Overlarge shifts can cause undefined behavior
template<bounded_integer Integer> requires(numeric_traits::min_value<Integer> >= constant<0>)
constexpr auto operator<<(std::byte const byte, Integer const number) {
	return byte << number.value();
}

// TODO: Overlarge shifts can cause undefined behavior
template<bounded_integer Integer> requires(numeric_traits::min_value<Integer> >= constant<0>)
constexpr auto operator>>(std::byte const byte, Integer const number) {
	return byte >> number.value();
}

} // namespace bounded

