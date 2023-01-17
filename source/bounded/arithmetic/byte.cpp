// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <compare>
#include <cstddef>

export module bounded.arithmetic.byte;

import bounded.comparison;
import bounded.homogeneous_equals;
import bounded.integer;
import bounded.is_bounded_integer;

import numeric_traits;
import std_module;

namespace bounded {

// TODO: Overlarge shifts can cause undefined behavior
export template<bounded_integer Integer> requires(numeric_traits::min_value<Integer> >= constant<0>)
constexpr auto operator<<(std::byte const byte, Integer const number) {
	return byte << number.value();
}

// TODO: Overlarge shifts can cause undefined behavior
export template<bounded_integer Integer> requires(numeric_traits::min_value<Integer> >= constant<0>)
constexpr auto operator>>(std::byte const byte, Integer const number) {
	return byte >> number.value();
}

} // namespace bounded

static_assert(homogeneous_equals(
	std::byte(0b00000000) << bounded::constant<0>,
	std::byte(0b00000000)
));
static_assert(homogeneous_equals(
	std::byte(0b00000000) >> bounded::constant<0>,
	std::byte(0b00000000)
));

static_assert(homogeneous_equals(
	std::byte(0b00000000) << bounded::constant<8>,
	std::byte(0b00000000)
));
static_assert(homogeneous_equals(
	std::byte(0b00000000) >> bounded::constant<8>,
	std::byte(0b00000000)
));

static_assert(homogeneous_equals(
	std::byte(0b00000001) << bounded::constant<0>,
	std::byte(0b00000001)
));
static_assert(homogeneous_equals(
	std::byte(0b00000001) >> bounded::constant<0>,
	std::byte(0b00000001)
));

static_assert(homogeneous_equals(
	std::byte(0b00000001) << bounded::constant<1>,
	std::byte(0b00000010)
));
static_assert(homogeneous_equals(
	std::byte(0b00000001) >> bounded::constant<1>,
	std::byte(0b00000000)
));

static_assert(homogeneous_equals(
	std::byte(0b00000001) << bounded::constant<8>,
	std::byte(0b00000000)
));
static_assert(homogeneous_equals(
	std::byte(0b00000001) >> bounded::constant<8>,
	std::byte(0b00000000)
));

static_assert(homogeneous_equals(
	std::byte(0b01001001) << bounded::constant<3>,
	std::byte(0b01001000)
));
static_assert(homogeneous_equals(
	std::byte(0b01001001) >> bounded::constant<3>,
	std::byte(0b00001001)
));
