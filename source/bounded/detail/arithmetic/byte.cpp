// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/arithmetic/byte.hpp>

#include <bounded/detail/class.hpp>

#include "../../homogeneous_equals.hpp"

namespace {

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

} // namespace
