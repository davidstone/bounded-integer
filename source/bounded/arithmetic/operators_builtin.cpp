// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.arithmetic.operators_builtin;

import bounded.arithmetic.operators;
import bounded.bounded_integer;
import bounded.comparison_builtin;
import bounded.homogeneous_equals;
import bounded.integer;

import std_module;

namespace bounded {

#define BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(symbol) \
export constexpr auto operator symbol(bounded_integer auto const lhs, builtin_integer auto const rhs) { \
	return (lhs symbol integer(rhs)).value(); \
} \
export constexpr auto operator symbol(builtin_integer auto const lhs, bounded_integer auto const rhs) { \
	return (integer(lhs) symbol rhs).value(); \
} \
\
export template<typename T, T rhs> \
constexpr auto operator symbol(bounded_integer auto const lhs, std::integral_constant<T, rhs>) { \
	return lhs symbol constant<rhs>; \
} \
export template<typename T, T lhs> \
constexpr auto operator symbol(std::integral_constant<T, lhs>, bounded_integer auto const rhs) { \
	return constant<lhs> symbol rhs; \
}


BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(+)
// binary minus uses the generic operator
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(*)
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(&)
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(/)
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(%)
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(<<)
BOUNDED_INTEGER_MIXED_OPERATOR_OVERLOADS(>>)

} // namespace bounded

static_assert(homogeneous_equals(
	bounded::integer<1, 10>(bounded::constant<9>) + std::integral_constant<int, 5>{},
	bounded::integer<6, 15>(bounded::constant<14>)
));

static_assert((bounded::constant<17> & 1) == 1);
static_assert((bounded::constant<18> & 1) == 0);

constexpr auto plus_equals = [](auto lhs, auto rhs) {
	return lhs += rhs;
};
constexpr auto minus_equals = [](auto lhs, auto rhs) {
	return lhs -= rhs;
};
constexpr auto multiplies_equals = [](auto lhs, auto rhs) {
	return lhs *= rhs;
};
constexpr auto divides_equals = [](auto lhs, auto rhs) {
	return lhs /= rhs;
};
constexpr auto modulus_equals = [](auto lhs, auto rhs) {
	return lhs %= rhs;
};

using integer_type = bounded::integer<-100, 100>;

constexpr auto check_plus(auto function) {
	return function(integer_type(bounded::constant<-5>), 5) == bounded::constant<0>;
}
static_assert(check_plus(std::plus()));
static_assert(check_plus(plus_equals));

constexpr auto check_minus(auto function) {
	return function(integer_type(bounded::constant<7>), 1) == bounded::constant<6>;
}
static_assert(check_minus(std::minus()));
static_assert(check_minus(minus_equals));

constexpr auto check_multiplies(auto function) {
	return function(integer_type(bounded::constant<3>), 4) == bounded::constant<12>;
}
static_assert(check_multiplies(std::multiplies()));
static_assert(check_multiplies(multiplies_equals));

constexpr auto check_divides(auto function) {
	return function(integer_type(bounded::constant<56>), 7) == bounded::constant<8>;
}
static_assert(check_divides(std::divides()));
static_assert(check_divides(divides_equals));

constexpr auto check_modulus(auto function) {
	return function(integer_type(bounded::constant<63>), 10) == bounded::constant<3>;
}
static_assert(check_modulus(std::modulus()));
static_assert(check_modulus(modulus_equals));

static_assert(plus_equals(0L, bounded::constant<4>) == 4);
static_assert(plus_equals(9U, bounded::constant<3>) == 12);
static_assert(minus_equals(9, bounded::constant<68>) == -59);
static_assert(divides_equals(-59, bounded::constant<10>) == -5);
static_assert(modulus_equals(-5, bounded::constant<4>) == -1);
