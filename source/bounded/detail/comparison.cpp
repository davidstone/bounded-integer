// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/comparison.hpp>

#include <bounded/detail/class.hpp>

#include <numeric_traits/min_max_value.hpp>

namespace {

constexpr auto a = bounded::integer<1, 10>(bounded::constant<5>);
static_assert(
	a == a,
	"Values do not equal themselves"
);
static_assert(
	a == 5,
	"Values do not equal their underlying value"
);
constexpr auto b = bounded::integer<4, 36346>(bounded::constant<5>);
static_assert(
	a == b,
	"Values do not equal equivalent other bounded::integer types"
);

static_assert(
	bounded::constant<5> != bounded::constant<6>,
	"5 should not equal 6"
);

static_assert(
	bounded::constant<-1> < bounded::constant<1>,
	"Small negative values should be less than small positive values."
);
constexpr intmax_t int_min = numeric_traits::min_value<int>;
constexpr intmax_t int_max = numeric_traits::max_value<int>;
static_assert(
	bounded::constant<int_min> < bounded::constant<int_max + 1>,
	"Large negative values should be less than large positive values."
);

// I have to use the preprocessor here to create a string literal
#define BOUNDED_INTEGER_SINGLE_COMPARISON(lhs, op, rhs, result) \
	static_assert( \
		(((lhs) op (rhs)) == (result)), \
		"Incorrect result for (" #lhs ") " #op " ( " #rhs ") == " #result \
	)

#define BOUNDED_INTEGER_COMPARISON(lhs, op, rhs) \
	BOUNDED_INTEGER_SINGLE_COMPARISON(bounded::constant<lhs>, op, bounded::constant<rhs>, ((lhs) op (rhs))); \
	BOUNDED_INTEGER_SINGLE_COMPARISON(bounded::constant<lhs>, op, rhs, ((lhs) op (rhs))); \
	BOUNDED_INTEGER_SINGLE_COMPARISON(lhs, op, bounded::constant<rhs>, ((lhs) op (rhs))); \
	BOUNDED_INTEGER_SINGLE_COMPARISON(bounded::constant<rhs>, op, bounded::constant<lhs>, ((rhs) op (lhs))); \
	BOUNDED_INTEGER_SINGLE_COMPARISON(bounded::constant<rhs>, op, lhs, ((rhs) op (lhs))); \
	BOUNDED_INTEGER_SINGLE_COMPARISON(rhs, op, bounded::constant<lhs>, ((rhs) op (lhs)))

BOUNDED_INTEGER_COMPARISON(-4, <=, -4);
BOUNDED_INTEGER_COMPARISON(-4, <=, 16);
BOUNDED_INTEGER_COMPARISON(16, <=, 400);
BOUNDED_INTEGER_COMPARISON(-17, <, 0);
BOUNDED_INTEGER_COMPARISON(-17, <, 17);
BOUNDED_INTEGER_COMPARISON(0, < , 17);
BOUNDED_INTEGER_COMPARISON(876, >=, 876);
BOUNDED_INTEGER_COMPARISON(876, >=, 367);
BOUNDED_INTEGER_COMPARISON(1LL << 50LL, >, 1LL << 30LL);
BOUNDED_INTEGER_COMPARISON(1LL << 50LL, >, 7);

#undef BOUNDED_INTEGER_COMPARISON
#undef BOUNDED_INTEGER_SINGLE_COMPARISON

auto non_constexpr_five = bounded::constant<5>;
auto non_constexpr_four = bounded::constant<4>;
static_assert(non_constexpr_five == non_constexpr_five, "operator== not constexpr for non-constexpr arguments.");
static_assert(non_constexpr_five != non_constexpr_four, "operator!= not constexpr for non-constexpr arguments.");
static_assert(non_constexpr_four < non_constexpr_five, "operator< not constexpr for non-constexpr arguments.");
static_assert(non_constexpr_four <= non_constexpr_four, "operator<= not constexpr for non-constexpr arguments.");

// Intentionally not constexpr
auto const zero = bounded::constant<0>;
auto const one = bounded::constant<1>;

static_assert(zero == zero);
static_assert(zero != one);
static_assert(zero < one);
static_assert(one > zero);
static_assert(zero <= one);
static_assert(one >= zero);

}	// namespace
