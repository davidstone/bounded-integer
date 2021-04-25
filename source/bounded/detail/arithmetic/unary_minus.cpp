// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/arithmetic/unary_minus.hpp>
#include <bounded/detail/class.hpp>

#include <numeric_traits/min_max_value.hpp>

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
	static_assert(homogeneous_equals(
		-bounded::constant<numeric_traits::max_value<Signed>>,
		bounded::constant<-numeric_traits::max_value<Signed>>
	));
	static_assert(homogeneous_equals(
		-bounded::constant<numeric_traits::min_value<Signed>>,
		// The outer cast to unsigned is to counteract integer promotion
		bounded::constant<static_cast<Unsigned>(-static_cast<Unsigned>(numeric_traits::min_value<Signed>))>
	));
	static_assert(homogeneous_equals(
		-(-bounded::constant<numeric_traits::min_value<Signed>>),
		bounded::constant<numeric_traits::min_value<Signed>>
	));
}
template<typename Signed, typename Unsigned, typename LargerSigned>
constexpr auto check_limits() {
	check_signed_limits<Signed, Unsigned>();
	static_assert(homogeneous_equals(
		-bounded::constant<numeric_traits::max_value<Unsigned>>,
		bounded::constant<-static_cast<LargerSigned>(numeric_traits::max_value<Unsigned>)>
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
	-bounded::integer<1, 10>(bounded::constant<9>),
	bounded::integer<-10, -1>(bounded::constant<-9>)
));

static_assert(homogeneous_equals(
	-bounded::integer<-3, 6>(bounded::constant<4>),
	bounded::integer<-6, 3>(bounded::constant<-4>)
));

}	// namespace
