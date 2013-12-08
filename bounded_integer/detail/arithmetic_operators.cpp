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

#include "arithmetic_operators.hpp"
#include "comparison_operators.hpp"
#include "numeric_limits.hpp"
#include "policy/throw_policy.hpp"

namespace {

constexpr bounded_integer<1, 10, throw_policy> const x(9);
static_assert(sizeof(x) == 1, "bounded_integer too big!");
constexpr bounded_integer<-3, 11, throw_policy> const y(x);
constexpr bounded_integer<-3, 11, throw_policy> const z(4);
static_assert(std::numeric_limits<decltype(z)>::is_signed, "bounded_integer with negative value in range should be signed.");

constexpr auto sum = x + z;
static_assert(std::numeric_limits<decltype(sum)>::min() == -2, "Minimum sum incorrect.");
static_assert(std::numeric_limits<decltype(sum)>::max() == 21, "Maximum sum incorrect.");
static_assert(sum == 13, "Calculated sum incorrect.");

constexpr auto difference = x - z;
static_assert(std::numeric_limits<decltype(difference)>::min() == -10, "Minimum difference incorrect.");
static_assert(std::numeric_limits<decltype(difference)>::max() == 13, "Maximum difference incorrect.");
static_assert(difference == 5, "Calculated difference incorrect.");

constexpr auto product = x * z;
static_assert(std::numeric_limits<decltype(product)>::min() == -30, "Minimum product incorrect.");
static_assert(std::numeric_limits<decltype(product)>::max() == 110, "Maximum product incorrect.");
static_assert(product == 36, "Calculated product incorrect.");

constexpr auto quotient = x / z;
static_assert(std::numeric_limits<decltype(quotient)>::min() == -10, "Minimum quotient incorrect.");
static_assert(std::numeric_limits<decltype(quotient)>::max() == 10, "Maximum quotient incorrect.");
static_assert(quotient == 2, "Calculated quotient incorrect.");

constexpr auto negation = -x;
static_assert(std::numeric_limits<decltype(negation)>::min() == -10, "Minimum quotient incorrect.");
static_assert(std::numeric_limits<decltype(negation)>::max() == -1, "Maximum quotient incorrect.");
static_assert(negation == -9, "Calculated negation incorrect.");

static_assert(quotient < product, "quotient should be less than product.");
static_assert(difference + 8 == sum, "difference + 8 should equal sum.");

constexpr auto positive = +x;
static_assert(positive == x, "Unary plus not a no-op.");

constexpr bounded_integer<0, 2, throw_policy> left_shift_lhs(1);
constexpr bounded_integer<0, 61, throw_policy> left_shift_rhs(3);
constexpr auto left_shift_result = left_shift_lhs << left_shift_rhs;
static_assert(std::numeric_limits<decltype(left_shift_result)>::min() == 0, "Minimum left shift result incorrect.");
static_assert(std::numeric_limits<decltype(left_shift_result)>::max() == (2ll << 61ll), "Minimum left shift result incorrect.");
static_assert(left_shift_result == (1 << 3), "Incorrect left shift result.");


// modulo

constexpr auto ten = make_bounded<10>();
constexpr auto eleven = make_bounded<11>();
constexpr auto ten_result = ten % eleven;
static_assert(ten_result == ten, "Incorrect modulo with divisor one greater");
static_assert(std::is_same<decltype(ten_result), decltype(ten)>::value, "Incorrect modulo type with divisor one greater");

constexpr auto nine = make_bounded<9>();
constexpr auto one = make_bounded<1>();
constexpr auto one_result = ten % nine;
static_assert(one_result == one, "Incorrect modulo with divisor one less");
static_assert(std::is_same<decltype(one_result), decltype(one)>::value, "Incorrect modulo type with divisor one less");

constexpr auto nine_result = nine % eleven;
static_assert(nine_result == nine, "Incorrect modulo with divisor two less");
static_assert(std::is_same<decltype(nine_result), decltype(nine)>::value, "Incorrect modulo type with divisor two less");

constexpr auto two = make_bounded<2>();
constexpr auto two_result = eleven % nine;
static_assert(two_result == two, "Incorrect modulo with divisor two greater");
static_assert(std::is_same<decltype(two_result), decltype(two)>::value, "Incorrect modulo type with divisor two greater");


constexpr bounded_integer<17, 23, throw_policy> positive_range(20);
constexpr bounded_integer<-54, -6, throw_policy> negative_range(-33);
constexpr auto positive_negative_result = positive_range % negative_range;
constexpr bounded_integer<0, 23, throw_policy> positive_negative(20 % -33);
static_assert(positive_negative_result == positive_negative, "Incorrect modulo with mixed signs");
static_assert(std::is_same<decltype(positive_negative_result), decltype(positive_negative)>::value, "Incorrect modulo type with mixed signs");

constexpr auto negative_positive_result = negative_range % positive_range;
constexpr bounded_integer<-22, 0, throw_policy> negative_positive(-33 % 20);
static_assert(negative_positive_result == negative_positive, "Incorrect modulo with mixed signs");
static_assert(std::is_same<decltype(negative_positive_result), decltype(negative_positive)>::value, "Incorrect modulo type with mixed signs");

constexpr auto result = bounded_integer<0, 10, null_policy>(10) % make_bounded<6>();
//	static_assert(static_cast<intmax_t>(std::numeric_limits<decltype(result)>::min()) == 0, "uh oh");
//	static_assert(static_cast<intmax_t>(std::numeric_limits<decltype(result)>::max()) == 10, "uh oh");
static_assert(result == 4, "wrong answer");

constexpr auto zero = make_bounded<0>();
constexpr auto zero_result = zero % make_bounded<1>();
static_assert(zero_result == zero, "Incorrect modulo with zero for the dividend");
static_assert(std::is_same<decltype(zero_result), decltype(zero)>::value, "Incorrect modulo type with zero for the dividend");
// auto undefined = 1 % zero;


}	// namespace
