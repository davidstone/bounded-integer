// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/arithmetic/unary_minus.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/typedefs.hpp>
#include "../../homogeneous_equals.hpp"

namespace {

static_assert(homogeneous_equals(
	-bounded::constant<0>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	-bounded::constant<1>,
	bounded::constant<-1>
));

template<typename Signed, typename Unsigned>
constexpr auto check_signed_limits() {
	// TODO: Make this test work when min() == -max()
	static_assert(homogeneous_equals(
		-bounded::constant<std::numeric_limits<Signed>::max()>,
		bounded::constant<-std::numeric_limits<Signed>::max()>
	));
	static_assert(homogeneous_equals(
		-bounded::constant<std::numeric_limits<Signed>::min()>,
		// The outer cast to unsigned is to counteract integer promotion
		bounded::constant<static_cast<Unsigned>(-static_cast<Unsigned>(std::numeric_limits<Signed>::min()))>
	));
}
template<typename Signed, typename Unsigned, typename LargerSigned>
constexpr auto check_limits() {
	check_signed_limits<Signed, Unsigned>();
	static_assert(homogeneous_equals(
		-bounded::constant<std::numeric_limits<Unsigned>::max()>,
		bounded::constant<-static_cast<LargerSigned>(std::numeric_limits<Unsigned>::max())>
	));
}

constexpr auto check_all_limits() {
	check_limits<std::int8_t, std::uint8_t, std::int16_t>();
	check_limits<std::int16_t, std::uint16_t, std::int32_t>();
	check_limits<std::int32_t, std::uint32_t, std::int64_t>();
	check_signed_limits<std::int64_t, std::uint64_t>();
}
static_assert((static_cast<void>(check_all_limits()), true));

static_assert(homogeneous_equals(
	-bounded::checked_integer<1, 10>(9, bounded::non_check),
	bounded::checked_integer<-10, -1>(-9, bounded::non_check)
));

}	// namespace
