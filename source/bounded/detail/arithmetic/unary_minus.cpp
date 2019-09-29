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
	using limits = std::numeric_limits<Signed>;
	static_assert(homogeneous_equals(
		-bounded::constant<limits::max()>,
		bounded::constant<-limits::max()>
	));
	static_assert(homogeneous_equals(
		-bounded::constant<limits::min()>,
		// The outer cast to unsigned is to counteract integer promotion
		bounded::constant<static_cast<Unsigned>(-static_cast<Unsigned>(limits::min()))>
	));
	static_assert(homogeneous_equals(
		-(-bounded::constant<limits::min()>),
		bounded::constant<limits::min()>
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
	#if defined BOUNDED_DETAIL_HAS_128_BIT
	check_limits<std::int64_t, std::uint64_t, bounded::detail::int128_t>();
	#else
	check_signed_limits<std::int64_t, std::uint64_t>();
	#endif
	return true;
}
static_assert(check_all_limits());

static_assert(homogeneous_equals(
	-bounded::checked_integer<1, 10>(9, bounded::non_check),
	bounded::checked_integer<-10, -1>(-9, bounded::non_check)
));

static_assert(homogeneous_equals(
	-bounded::checked_integer<-3, 6>(4, bounded::non_check),
	bounded::checked_integer<-6, 3>(-4, bounded::non_check)
));

}	// namespace
