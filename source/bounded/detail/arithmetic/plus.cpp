// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/arithmetic/plus.hpp>
#include <bounded/detail/arithmetic/operators_builtin.hpp>
#include <bounded/detail/policy/throw_policy.hpp>
#include <bounded/detail/policy/modulo_policy.hpp>
#include <bounded/detail/class.hpp>
#include "extreme_values.hpp"
#include "../../homogeneous_equals.hpp"

namespace {

static_assert(homogeneous_equals(
	bounded::constant<0> + bounded::constant<0>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<0> + bounded::constant<1>,
	bounded::constant<1>
));
static_assert(homogeneous_equals(
	bounded::constant<-1> + bounded::constant<1>,
	bounded::constant<0>
));

using bounded::detail::max_unsigned_t;

static_assert(homogeneous_equals(
	signed_max + bounded::constant<1>,
	bounded::constant<static_cast<max_unsigned_t>(signed_max) + 1>
));

static_assert(homogeneous_equals(
	signed_max + signed_max,
	bounded::constant<static_cast<max_unsigned_t>(signed_max) + static_cast<max_unsigned_t>(signed_max)>
));

// Do not use bounded's operator- for this test
constexpr auto negative_signed_min = bounded::constant<-static_cast<max_unsigned_t>(signed_min)>;
static_assert(negative_signed_min == signed_max + bounded::constant<1>);
static_assert(homogeneous_equals(
	signed_min + negative_signed_min,
	bounded::constant<0>
));

static_assert(homogeneous_equals(
	unsigned_max + signed_min,
	bounded::constant<static_cast<max_unsigned_t>(unsigned_max) + static_cast<max_unsigned_t>(signed_min)>
));

static_assert(homogeneous_equals(
	bounded::checked_integer<1, 10>(9, bounded::non_check) + bounded::checked_integer<-3, 11>(4, bounded::non_check),
	bounded::integer<-2, 21>(13, bounded::non_check)
));
static_assert(homogeneous_equals(
	bounded::checked_integer<1, 10>(9, bounded::non_check) + bounded::wrapping_integer<-3, 11>(4, bounded::non_check),
	bounded::integer<-2, 21>(13, bounded::non_check)
));
static_assert(homogeneous_equals(
	bounded::checked_integer<1, 10>(9, bounded::non_check) + std::integral_constant<int, 5>{},
	bounded::integer<6, 15>(14, bounded::non_check)
));

}	// namespace
