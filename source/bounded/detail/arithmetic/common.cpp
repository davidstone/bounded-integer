// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#undef NDEBUG

#include <bounded/detail/arithmetic/common.hpp>
#include <bounded/detail/arithmetic/operators_builtin.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/comparison_mixed.hpp>
#include <bounded/detail/typedefs.hpp>

namespace {

template<typename LHS, typename RHS>
constexpr auto plus_equals(LHS lhs, RHS rhs) {
	return lhs += rhs;
}
template<typename LHS, typename RHS>
constexpr auto minus_equals(LHS lhs, RHS rhs) {
	return lhs -= rhs;
}
template<typename LHS, typename RHS>
constexpr auto times_equals(LHS lhs, RHS rhs) {
	return lhs *= rhs;
}
template<typename LHS, typename RHS>
constexpr auto divide_equals(LHS lhs, RHS rhs) {
	return lhs /= rhs;
}
template<typename LHS, typename RHS>
constexpr auto modulo_equals(LHS lhs, RHS rhs) {
	return lhs %= rhs;
}

constexpr auto check_compound_arithmetic() {
	using x_type = bounded::checked_integer<0, 10>;
	using y_type = bounded::checked_integer<-10, 10>;
	static_assert(plus_equals(x_type(bounded::constant<5>), bounded::constant<5>) == bounded::constant<10>);
	static_assert(plus_equals(y_type(-bounded::constant<5>), 5) == bounded::constant<0>);
	static_assert(plus_equals(y_type(bounded::constant<0>), x_type(bounded::constant<10>)) == bounded::constant<10>);
	
	using z_type = bounded::checked_integer<-1000, 1000>;
	static_assert(plus_equals(z_type(bounded::constant<0>), x_type(bounded::constant<10>)) == bounded::constant<10>);
	static_assert(times_equals(z_type(bounded::constant<10>), x_type(bounded::constant<10>) - 5) == bounded::constant<50>);
	static_assert(times_equals(z_type(bounded::constant<10>), x_type(bounded::constant<10>) - 5) == bounded::constant<50>);
	static_assert(minus_equals(z_type(bounded::constant<50>), 1) == bounded::constant<49>);
	static_assert(divide_equals(z_type(bounded::constant<49>), 7) == bounded::constant<7>);
	static_assert(modulo_equals(x_type(bounded::constant<10>), bounded::constant<6>) == bounded::constant<4>);

	static_assert(plus_equals(static_cast<short>(0), bounded::constant<4>) == 4);
	static_assert(plus_equals(static_cast<unsigned>(9), bounded::constant<3>) == 12);
	static_assert(minus_equals(9, bounded::constant<68>) == -59);
	static_assert(divide_equals(-59, y_type(bounded::constant<10>)) == -5);
	static_assert(modulo_equals(-5, bounded::constant<4>) == -1);

	auto x = x_type(bounded::constant<4>);
	auto const pre_increment = ++x;
	assert(pre_increment == bounded::constant<5>);
	assert(x == bounded::constant<5>);
	x = 0;
	assert(x == 0);
	auto z = z_type(bounded::constant<0>);
	auto const post_increment = z++;
	assert(post_increment == 0);
	assert(z == 1);

	static_assert(minus_equals(bounded::clamped_integer<0, 10>(bounded::constant<5>), bounded::constant<20>) == bounded::constant<0>);
	
	return true;
}
static_assert(check_compound_arithmetic());

}	// namespace
