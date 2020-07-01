// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/arithmetic/multiplies.hpp>
#include <bounded/detail/arithmetic/plus.hpp>
#include <bounded/detail/arithmetic/unary_minus.hpp>
#include <bounded/detail/policy/throw_policy.hpp>
#include <bounded/detail/policy/clamp_policy.hpp>
#include <bounded/detail/class.hpp>
#include "extreme_values.hpp"
#include "../../homogeneous_equals.hpp"

namespace {

static_assert(homogeneous_equals(
	bounded::constant<0> * bounded::constant<0>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<0> * bounded::constant<1000>,
	bounded::constant<0>
));

static_assert(homogeneous_equals(
	signed_max * bounded::constant<1>,
	signed_max
));
static_assert(homogeneous_equals(
	signed_min * bounded::constant<1>,
	signed_min
));
static_assert(homogeneous_equals(
	unsigned_max * bounded::constant<1>,
	unsigned_max
));
static_assert(homogeneous_equals(
	signed_max * bounded::constant<-1>,
	-signed_max
));
static_assert(homogeneous_equals(
	signed_min * bounded::constant<-1>,
	-signed_min
));
static_assert(homogeneous_equals(
	signed_max * bounded::constant<2>,
	signed_max + signed_max
));
static_assert(homogeneous_equals(
	-signed_min * bounded::constant<-1>,
	signed_min
));

#if BOUNDED_DETAIL_HAS_128_BIT

static_assert(homogeneous_equals(
	bounded::constant<1'000'000'000'000'000'000> * bounded::constant<1'000'000'000'000'000'000>,
	bounded::constant<bounded::detail::uint128_t(1'000'000'000'000'000'000) * bounded::detail::uint128_t(1'000'000'000'000'000'000)>
));

#endif

static_assert(homogeneous_equals(
	bounded::integer<-3, 3>(1, bounded::non_check) * bounded::integer<0, 1>(1, bounded::non_check),
	bounded::integer<-3, 3>(1, bounded::non_check)
));
static_assert(homogeneous_equals(
	bounded::checked_integer<1, 10>(9, bounded::non_check) * bounded::checked_integer<-3, 11>(4, bounded::non_check),
	bounded::integer<-30, 110>(36, bounded::non_check)
));
static_assert(homogeneous_equals(
	bounded::checked_integer<1, 10>(9, bounded::non_check) * bounded::clamped_integer<-3, 11>(4, bounded::non_check),
	bounded::integer<-30, 110>(36, bounded::non_check)
));

}	// namespace
