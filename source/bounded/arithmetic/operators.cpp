// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/arithmetic/common_arithmetic.hpp>
#include <bounded/assert.hpp>

#include <operators/compound_assignment.hpp>

export module bounded.arithmetic.operators;

export import bounded.arithmetic.bitwise_and;
export import bounded.arithmetic.byte;
export import bounded.arithmetic.divides;
export import bounded.arithmetic.extreme_values;
export import bounded.arithmetic.increment_decrement;
export import bounded.arithmetic.left_shift;
export import bounded.arithmetic.modulus;
export import bounded.arithmetic.multiplies;
export import bounded.arithmetic.plus;
export import bounded.arithmetic.pointer;
export import bounded.arithmetic.right_shift;
export import bounded.arithmetic.unary_minus;
export import bounded.arithmetic.unary_plus;
import bounded.comparison;
import bounded.homogeneous_equals;
import bounded.integer;

import numeric_traits;
import operators;
import std_module;

namespace bounded {

BOUNDED_COMMON_ARITHMETIC(export)

} // namespace bounded

namespace {

static_assert(homogeneous_equals(
	bounded::constant<0> - bounded::constant<0>,
	bounded::constant<0>
));
static_assert(homogeneous_equals(
	bounded::constant<0> - bounded::constant<1>,
	bounded::constant<-1>
));
static_assert(homogeneous_equals(
	unsigned_max - signed_max,
	bounded::constant<static_cast<numeric_traits::max_unsigned_t>(unsigned_max) - static_cast<numeric_traits::max_unsigned_t>(signed_max)>
));
static_assert(homogeneous_equals(
	bounded::integer<1, 10>(bounded::constant<9>) - bounded::integer<-3, 11>(bounded::constant<4>),
	bounded::integer<-10, 13>(bounded::constant<5>)
));

constexpr auto plus_equals(auto lhs, auto rhs) {
	return lhs += rhs;
}
constexpr auto minus_equals(auto lhs, auto rhs) {
	return lhs -= rhs;
}
constexpr auto times_equals(auto lhs, auto rhs) {
	return lhs *= rhs;
}
constexpr auto divide_equals(auto lhs, auto rhs) {
	return lhs /= rhs;
}
constexpr auto modulo_equals(auto lhs, auto rhs) {
	return lhs %= rhs;
}

using x_type = bounded::integer<0, 10>;
using y_type = bounded::integer<-10, 10>;
using z_type = bounded::integer<-1000, 1000>;

static_assert(plus_equals(x_type(bounded::constant<5>), bounded::constant<5>) == bounded::constant<10>);
static_assert(plus_equals(y_type(bounded::constant<0>), x_type(bounded::constant<10>)) == bounded::constant<10>);
static_assert(plus_equals(z_type(bounded::constant<0>), x_type(bounded::constant<10>)) == bounded::constant<10>);
static_assert(times_equals(z_type(bounded::constant<10>), x_type(bounded::constant<5>)) == bounded::constant<50>);
static_assert(modulo_equals(x_type(bounded::constant<10>), bounded::constant<6>) == bounded::constant<4>);

static_assert([]{
	auto z = z_type(bounded::constant<0>);
	auto const post_increment = z++;
	BOUNDED_ASSERT(post_increment == bounded::constant<0>);
	BOUNDED_ASSERT(z == bounded::constant<1>);
	return true;
}());

} // namespace