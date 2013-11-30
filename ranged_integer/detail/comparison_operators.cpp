// Verify that the header can stand on its own, run tests
// Copyright (C) 2013 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "comparison_operators.hpp"
#include "numeric_limits.hpp"
#include "policy/throw_policy.hpp"

namespace {

constexpr ranged_integer<1, 10, null_policy> a(5);
static_assert(a == a, "Values do not equal themselves");
static_assert(a == 5, "Values do not equal their underlying value");
constexpr ranged_integer<4, 36346, throw_policy> b(5);
static_assert(a == b, "Values do not equal equivalent other ranged_integer types");

static_assert(make_ranged<5>() != make_ranged<6>(), "5 should not equal 6");

constexpr auto one = make_ranged<1>();
static_assert(!std::numeric_limits<decltype(one)>::is_signed, "Value should be unsigned for this test.");
constexpr auto negative_one = make_ranged<-1>();
static_assert(std::numeric_limits<decltype(negative_one)>::is_signed, "Value should be signed for this test.");
static_assert(negative_one < one, "Small negative values should be less than small positive values.");
constexpr intmax_t int_min = std::numeric_limits<int>::min();
constexpr intmax_t int_max = std::numeric_limits<int>::max();
static_assert(make_ranged<int_min>() < make_ranged<int_max + 1>(), "Large negative values should be less than large positive values.");

// I have to use the preprocessor here to create a string literal
#define RANGED_INTEGER_SINGLE_COMPARISON(op, a, b) \
	static_assert((make_ranged<a>() op make_ranged<b>()), "Incorrect result for (" #a ") " #op " ( "#b ")")

#define RANGED_INTEGER_COMPARISON(op, a, b, c) \
	RANGED_INTEGER_SINGLE_COMPARISON(op, a, b); \
	RANGED_INTEGER_SINGLE_COMPARISON(op, b, c); \
	RANGED_INTEGER_SINGLE_COMPARISON(op, a, c); \
	static_assert(!(make_ranged<c>() op make_ranged<a>()), "Incorrect result for !((" #c ") " #op " (" #a "))")

RANGED_INTEGER_COMPARISON(<=, -4, -4, 16);
RANGED_INTEGER_COMPARISON(<, -17, 0, 17);
RANGED_INTEGER_COMPARISON(>=, 876, 876, 367);
RANGED_INTEGER_COMPARISON(>, 1LL << 50LL, 1LL << 30LL, 7);

#undef RANGED_INTEGER_COMPARISON
#undef RANGED_INTEGER_SINGLE_COMPARISON

}	// namespace
