// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/arithmetic/modulus.hpp>
#include <bounded/detail/class.hpp>
#include "extreme_values.hpp"
#include "../../homogeneous_equals.hpp"

namespace {

static_assert(homogeneous_equals(
	bounded::constant<0> % bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<10> % bounded::constant<11>,
	bounded::constant<10>
));
static_assert(homogeneous_equals(
	bounded::constant<10> % bounded::constant<9>,
	bounded::constant<1>
));
static_assert(homogeneous_equals(
	bounded::constant<9> % bounded::constant<11>,
	bounded::constant<9>
));
static_assert(homogeneous_equals(
	bounded::constant<11> % bounded::constant<9>,
	bounded::constant<2>
));
static_assert(homogeneous_equals(
	bounded::constant<13> % bounded::constant<6>,
	bounded::constant<1>
));

static_assert(homogeneous_equals(
	signed_min % bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	signed_max % bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	unsigned_max % bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::integer<17, 23>(20, bounded::non_check) % bounded::integer<-54, -6>(-33, bounded::non_check),
	bounded::integer<0, 23>(20 % -33, bounded::non_check)
));
static_assert(homogeneous_equals(
	bounded::integer<-54, -6>(-33, bounded::non_check) % bounded::integer<17, 23>(20, bounded::non_check),
	bounded::integer<-22, 0>(-33 % 20, bounded::non_check)
));
static_assert(homogeneous_equals(
	bounded::integer<-22, 0>(-33 % 20, bounded::non_check) % bounded::integer<0, 23>(20 % -33, bounded::non_check),
	bounded::integer<-22, 0>(-13, bounded::non_check)
));
static_assert(homogeneous_equals(
	bounded::integer<0, 10>(10, bounded::non_check) % bounded::constant<6>,
	bounded::integer<0, 5>(4, bounded::non_check)
));
static_assert(homogeneous_equals(
	bounded::constant<0> % bounded::constant<1>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::integer<73, 76>(73, bounded::non_check) % bounded::integer<7, 8>(7, bounded::non_check),
	bounded::integer<1, 6>(73 % 7, bounded::non_check)
));
static_assert(homogeneous_equals(
	bounded::integer<74, 75>(74, bounded::non_check) % bounded::integer<7, 8>(7, bounded::non_check),
	bounded::integer<2, 5>(74 % 7, bounded::non_check)
));

// auto undefined = 1 % zero;

#if 0	
// The algorithm to compute the bounds currently runs in O(n) compile time,
// and thus this test exceed's the constexpr evaluation limits.
constexpr auto bounded_max_range = bounded::integer(bounded::min_value<bounded::detail::signed_max_t>);
static_assert(homogeneous_equals(
	bounded_max_range % bounded_max_range,
	bounded::integer<bounded::min_value<signed_max_t> + 1, -(bounded::min_value<signed_max_t> + 1)>(0)
));
#endif

}	// namespace
