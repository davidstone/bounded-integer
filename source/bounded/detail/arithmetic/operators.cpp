// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/arithmetic/operators.hpp>
#include <bounded/detail/arithmetic/operators_builtin.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/comparison_mixed.hpp>

#include "extreme_values.hpp"
#include "../../homogeneous_equals.hpp"
#include "../../../test_assert.hpp"

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
	bounded::constant<static_cast<bounded::detail::max_unsigned_t>(unsigned_max) - static_cast<bounded::detail::max_unsigned_t>(signed_max)>
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

constexpr auto check_compound_arithmetic() {
	using x_type = bounded::integer<0, 10>;
	using y_type = bounded::integer<-10, 10>;
	static_assert(plus_equals(x_type(bounded::constant<5>), bounded::constant<5>) == bounded::constant<10>);
	static_assert(plus_equals(y_type(-bounded::constant<5>), 5) == bounded::constant<0>);
	static_assert(plus_equals(y_type(bounded::constant<0>), x_type(bounded::constant<10>)) == bounded::constant<10>);
	
	using z_type = bounded::integer<-1000, 1000>;
	static_assert(plus_equals(z_type(bounded::constant<0>), x_type(bounded::constant<10>)) == bounded::constant<10>);
	static_assert(times_equals(z_type(bounded::constant<10>), x_type(bounded::constant<10>) - 5) == bounded::constant<50>);
	static_assert(times_equals(z_type(bounded::constant<10>), x_type(bounded::constant<10>) - 5) == bounded::constant<50>);
	static_assert(minus_equals(z_type(bounded::constant<50>), 1) == bounded::constant<49>);
	static_assert(divide_equals(z_type(bounded::constant<49>), 7) == bounded::constant<7>);
	static_assert(modulo_equals(x_type(bounded::constant<10>), bounded::constant<6>) == bounded::constant<4>);

	static_assert(plus_equals(static_cast<long>(0), bounded::constant<4>) == 4);
	static_assert(plus_equals(static_cast<unsigned>(9), bounded::constant<3>) == 12);
	static_assert(minus_equals(9, bounded::constant<68>) == -59);
	static_assert(divide_equals(-59, y_type(bounded::constant<10>)) == -5);
	static_assert(modulo_equals(-5, bounded::constant<4>) == -1);

	auto x = x_type(bounded::constant<4>);
	auto const pre_increment = ++x;
	BOUNDED_TEST(pre_increment == bounded::constant<5>);
	BOUNDED_TEST(x == bounded::constant<5>);
	x = 0;
	BOUNDED_TEST(x == 0);
	auto z = z_type(bounded::constant<0>);
	auto const post_increment = z++;
	BOUNDED_TEST(post_increment == 0);
	BOUNDED_TEST(z == 1);

	return true;
}
static_assert(check_compound_arithmetic());

}	// namespace