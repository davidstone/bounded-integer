// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/arithmetic/plus.hpp>
#include <bounded/detail/arithmetic/operators_builtin.hpp>
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
	bounded::integer<1, 10>(bounded::constant<9>) + bounded::integer<-3, 11>(bounded::constant<4>),
	bounded::integer<-2, 21>(bounded::constant<13>)
));
static_assert(homogeneous_equals(
	bounded::integer<1, 10>(bounded::constant<9>) + std::integral_constant<int, 5>{},
	bounded::integer<6, 15>(bounded::constant<14>)
));

}	// namespace
