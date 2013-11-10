// Ranged integer type tests
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

#include "detail/stream.hpp"
#include "ranged_integer.hpp"
#include "detail/common_type.hpp"
#include "detail/math.hpp"
#include "detail/numeric_limits.hpp"
#include <cassert>
#include <sstream>

namespace {

void check_comparison() {
	constexpr native_integer<1, 10> a(5);
	static_assert(a == a, "Values do not equal themselves");
	static_assert(a == 5, "Values do not equal their underlying value");
	constexpr checked_integer<4, 36346> b(5);
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
	#define RANGED_INTEGER_SINGLE_COMPARISON(operator, a, b) \
		static_assert((make_ranged<a>() operator make_ranged<b>()), "Incorrect result for (" #a ") " #operator " ( "#b ")")

	#define RANGED_INTEGER_COMPARISON(operator, a, b, c) \
		do { \
			RANGED_INTEGER_SINGLE_COMPARISON(operator, a, b); \
			RANGED_INTEGER_SINGLE_COMPARISON(operator, b, c); \
			RANGED_INTEGER_SINGLE_COMPARISON(operator, a, c); \
			static_assert(!(make_ranged<c>() operator make_ranged<a>()), "Incorrect result for !((" #c ") " #operator " (" #a "))"); \
		} while (false)

	RANGED_INTEGER_COMPARISON(<=, -4, -4, 16);
	RANGED_INTEGER_COMPARISON(<, -17, 0, 17);
	RANGED_INTEGER_COMPARISON(>=, 876, 876, 367);
	RANGED_INTEGER_COMPARISON(>, 1LL << 50LL, 1LL << 30LL, 7);
	
	#undef RANGED_INTEGER_COMPARISON
	#undef RANGED_INTEGER_SINGLE_COMPARISON
}

template<typename integer>
void check_numeric_limits() {
	using int_limits = std::numeric_limits<integer>;
	using ranged_t = checked_integer<int_limits::min(), int_limits::max()>;
	using ranged_limits = std::numeric_limits<ranged_t>;
	static_assert(sizeof(ranged_t) == sizeof(integer), "checked_integer wrong size.");

	// I have to use the preprocessor here to create a string literal
	#define RANGED_INTEGER_CHECK_CONDITION(condition) \
		static_assert(int_limits::condition == ranged_limits::condition, #condition " is wrong.")
	RANGED_INTEGER_CHECK_CONDITION(is_specialized);
	RANGED_INTEGER_CHECK_CONDITION(is_integer);
	RANGED_INTEGER_CHECK_CONDITION(is_exact);
	RANGED_INTEGER_CHECK_CONDITION(has_infinity);
	RANGED_INTEGER_CHECK_CONDITION(has_quiet_NaN);
	RANGED_INTEGER_CHECK_CONDITION(has_signaling_NaN);
	RANGED_INTEGER_CHECK_CONDITION(has_denorm);
	RANGED_INTEGER_CHECK_CONDITION(has_denorm_loss);
	RANGED_INTEGER_CHECK_CONDITION(is_iec559);
	RANGED_INTEGER_CHECK_CONDITION(is_bounded);
	// is_modulo intentionally left out because ranged_integer may differ from
	// the behavior of built-ins. Instead, just instantiate it with a
	// do-nothing test to verify that it compiles.
	static_cast<void>(ranged_limits::is_modulo);
	// RANGED_INTEGER_CHECK_CONDITION(is_modulo);
	RANGED_INTEGER_CHECK_CONDITION(radix);
	RANGED_INTEGER_CHECK_CONDITION(digits);
	RANGED_INTEGER_CHECK_CONDITION(digits10);
	RANGED_INTEGER_CHECK_CONDITION(max_digits10);
	RANGED_INTEGER_CHECK_CONDITION(min_exponent);
	RANGED_INTEGER_CHECK_CONDITION(min_exponent10);
	RANGED_INTEGER_CHECK_CONDITION(max_exponent);
	RANGED_INTEGER_CHECK_CONDITION(max_exponent10);
	RANGED_INTEGER_CHECK_CONDITION(traps);
	RANGED_INTEGER_CHECK_CONDITION(tinyness_before);
	#undef RANGED_INTEGER_CHECK_CONDITION

	#define RANGED_INTEGER_CHECK_FUNCTION(function) \
		static_assert(int_limits::function() == ranged_limits::function(), #function "() is wrong.")
	// Some of the functions are meaningless for integers, so I do not compare
	#define RANGED_INTEGER_CHECK_MEANINGLESS_FUNCTION(function) \
		static_cast<void>(ranged_limits::function())
	RANGED_INTEGER_CHECK_FUNCTION(min);
	RANGED_INTEGER_CHECK_FUNCTION(lowest);
	RANGED_INTEGER_CHECK_FUNCTION(max);
	RANGED_INTEGER_CHECK_MEANINGLESS_FUNCTION(epsilon);
	RANGED_INTEGER_CHECK_MEANINGLESS_FUNCTION(round_error);
	RANGED_INTEGER_CHECK_MEANINGLESS_FUNCTION(infinity);
	RANGED_INTEGER_CHECK_MEANINGLESS_FUNCTION(quiet_NaN);
	RANGED_INTEGER_CHECK_MEANINGLESS_FUNCTION(signaling_NaN);
	RANGED_INTEGER_CHECK_MEANINGLESS_FUNCTION(denorm_min);
	#undef RANGED_INTEGER_CHECK_MEANINGLESS_FUNCTION
	#undef RANGED_INTEGER_CHECK_FUNCTION
}
void check_numeric_limits_all() {
	check_numeric_limits<int8_t>();
	check_numeric_limits<uint8_t>();
	check_numeric_limits<int16_t>();
	check_numeric_limits<uint16_t>();
	check_numeric_limits<int32_t>();
	check_numeric_limits<uint32_t>();
	check_numeric_limits<int64_t>();
	// Currently does not support unsigned types equal to uintmax_t
	// check_numeric_limits<uint64_t>();
	
	static_assert(std::numeric_limits<native_integer<1, 1000>>::digits == 0, "Meaningless digits not 0.");
}

void check_modulo();

void check_arithmetic() {
	constexpr checked_integer<1, 10> const x(9);
	static_assert(sizeof(x) == 1, "checked_integer too big!");
	constexpr checked_integer<-3, 11> const y(x);
	constexpr checked_integer<-3, 11> const z(4);
	static_assert(std::numeric_limits<decltype(z)>::is_signed, "ranged_integer with negative value in range should be signed.");

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

	// constexpr checked_integer<2, 8> const z(x);
	// checked_integer<13, 63> const non_overlapping(x);
	
	check_modulo();
}

void check_modulo() {
	constexpr auto ten = make_ranged<10>();
	constexpr auto eleven = make_ranged<11>();
	constexpr auto ten_result = ten % eleven;
	static_assert(ten_result == ten, "Incorrect modulo with divisor one greater");
	static_assert(std::is_same<decltype(ten_result), decltype(ten)>::value, "Incorrect modulo type with divisor one greater");

	constexpr auto nine = make_ranged<9>();
	constexpr auto one = make_ranged<1>();
	constexpr auto one_result = ten % nine;
	static_assert(one_result == one, "Incorrect modulo with divisor one less");
	static_assert(std::is_same<decltype(one_result), decltype(one)>::value, "Incorrect modulo type with divisor one less");

	constexpr auto nine_result = nine % eleven;
	static_assert(nine_result == nine, "Incorrect modulo with divisor two less");
	static_assert(std::is_same<decltype(nine_result), decltype(nine)>::value, "Incorrect modulo type with divisor two less");

	constexpr auto two = make_ranged<2>();
	constexpr auto two_result = eleven % nine;
	static_assert(two_result == two, "Incorrect modulo with divisor two greater");
	static_assert(std::is_same<decltype(two_result), decltype(two)>::value, "Incorrect modulo type with divisor two greater");
	
	
	constexpr checked_integer<17, 23> positive_range(20);
	constexpr checked_integer<-54, -6> negative_range(-33);
	constexpr auto positive_negative_result = positive_range % negative_range;
	constexpr checked_integer<0, 23> positive_negative(20 % -33);
	static_assert(positive_negative_result == positive_negative, "Incorrect modulo with mixed signs");
	static_assert(std::is_same<decltype(positive_negative_result), decltype(positive_negative)>::value, "Incorrect modulo type with mixed signs");
	
	constexpr auto negative_positive_result = negative_range % positive_range;
	constexpr checked_integer<-22, 0> negative_positive(-33 % 20);
	static_assert(negative_positive_result == negative_positive, "Incorrect modulo with mixed signs");
	static_assert(std::is_same<decltype(negative_positive_result), decltype(negative_positive)>::value, "Incorrect modulo type with mixed signs");
	
	constexpr auto result = native_integer<0, 10>(10) % make_ranged<6>();
//	static_assert(static_cast<intmax_t>(std::numeric_limits<decltype(result)>::min()) == 0, "uh oh");
//	static_assert(static_cast<intmax_t>(std::numeric_limits<decltype(result)>::max()) == 10, "uh oh");
	static_assert(result == 4, "wrong answer");
	
	constexpr auto zero = make_ranged<0>();
	constexpr auto zero_result = zero % make_ranged<1>();
	static_assert(zero_result == zero, "Incorrect modulo with zero for the dividend");
	static_assert(std::is_same<decltype(zero_result), decltype(zero)>::value, "Incorrect modulo type with zero for the dividend");
	// auto undefined = 1 % zero;
}

void check_compound_arithmetic() {
	native_integer<0, 10> x(5);
	x += 5;
	assert(x == 10);
	checked_integer<-10, 10> y(-5);
	y += 5;
	assert(y == 0);
	y += x;	
	assert(y == 10);
	checked_integer<-1000, 1000> z(0);
	z += y;
	assert(z == 10);
	z *= x - 5;
	assert(z == 50);
	z -= 1;
	assert(z == 49);
	z /= 7;
	assert(z == 7);
	z = 0;
	assert(z == 0);
	x %= make_ranged<6>();
	assert(x == 4);
	
	short s = 0;
	s += make_ranged<4>();
	assert(s == 4);
	int i = 9;
	i -= make_ranged(68);
	assert(i == -59);
	long l = -7;
	l *= z;
	assert(l == 0);
	i /= y;
	assert(i == -5);
	i %= make_ranged(6);
	assert(i == -5);
	i %= make_ranged<4>();
	assert(i == -1);

	assert(++x == 5);
	assert(x == 5);
	assert(z++ == 0);
	assert(z == 1);
}

template<typename Initial, intmax_t initial_value, typename Expected, intmax_t expected_value>
void check_absolute_value() {
	constexpr Initial value(initial_value);
	constexpr auto absolute = abs(value);
	constexpr Expected expected_absolute(expected_value);
	static_assert(std::is_same<decltype(expected_absolute), decltype(absolute)>::value, "Absolute value returns the wrong type.");
	static_assert(absolute == expected_absolute, "Absolute value returns the wrong value.");
}

void check_math() {
	check_absolute_value<checked_integer<-10, 4>, -5, checked_integer<0, 10>, 5>();
	check_absolute_value<checked_integer<-10, -10>, -10, checked_integer<10, 10>, 10>();
	check_absolute_value<checked_integer<0, 0>, 0, checked_integer<0, 0>, 0>();
	check_absolute_value<checked_integer<-7, 450>, -1, checked_integer<0, 450>, 1>();
	check_absolute_value<checked_integer<-7, 450>, 1, checked_integer<0, 450>, 1>();
}

void check_common_type() {
	using type1 = ranged_integer<1, 5, null_policy>;
	using type2 = ranged_integer<3, 10, null_policy>;
	using common_type2 = common_type_t<type1, type2>;
	using expected_type2 = ranged_integer<1, 10, null_policy>;
	static_assert(std::is_same<expected_type2, common_type2>::value, "common_type wrong for 2 values.");
	using type3 = ranged_integer<-5, -5, null_policy>;
	using common_type3 = common_type_t<type1, type2, type3>;
	using expected_type3 = ranged_integer<-5, 10, null_policy>;
	static_assert(std::is_same<expected_type3, common_type3>::value, "common_type wrong for 3 values.");
	using type4 = ranged_integer<0, 0, null_policy>;
	using common_type4 = common_type_t<type1, type2, type3, type4>;
	using expected_type4 = ranged_integer<-5, 10, null_policy>;
	static_assert(std::is_same<expected_type4, common_type4>::value, "common_type wrong for 4 values.");
}

void check_throw_policy() {
	throw_on_overflow<0, 10> throw_policy;
	try {
		throw_policy(20);
		assert(false);
	}
	catch (std::overflow_error const &) {
	}
	try {
		throw_policy(-6);
		assert(false);
	}
	catch (std::underflow_error const &) {
	}
}

void check_clamp_policy() {
	static constexpr intmax_t minimum = 27;
	static constexpr intmax_t maximum = 567;
	constexpr clamp_on_overflow<minimum, maximum> clamp_policy;
	static_assert(clamp_policy(20) == minimum, "Failure to properly clamp lesser positive values.");
	static_assert(clamp_policy(-25) == minimum, "Failure to properly clamp negative values to a positive value.");
	static_assert(clamp_policy(1000) == maximum, "Failure to properly clamp greater positive values.");
	
	using type = clamped_integer<-100, 100>;
	constexpr auto initial = std::numeric_limits<type::underlying_type>::max() + 1;
	constexpr type value(initial);
	static_assert(value == std::numeric_limits<type>::max(), "Fail to clamp value when the source type is larger than the destination type.");
}

void check_policies() {
	check_throw_policy();
	check_clamp_policy();
}

void check_literals() {
	// I have to use the preprocessor here to create an integer literal
	#define RANGED_INTEGER_CHECK_LITERAL(x) \
		do { \
			constexpr auto value = x ## _ri; \
			using value_type = decltype(value); \
			\
			static_assert(std::numeric_limits<value_type>::min() == std::numeric_limits<value_type>::max(), "Literal does not have a min possible value equal to a max possible value."); \
			static_assert(std::numeric_limits<value_type>::min() == value, "Literal does not have a value equal to the range."); \
			\
			static_assert(value == static_cast<value_type::underlying_type>(x), "Inaccurate value of " #x " (cast x)"); \
			static_assert(static_cast<decltype(x)>(value) == x, "Inaccurate value of " #x " (cast value)"); \
		} while(false)
	RANGED_INTEGER_CHECK_LITERAL(0);
	RANGED_INTEGER_CHECK_LITERAL(1);
	RANGED_INTEGER_CHECK_LITERAL(10);
	RANGED_INTEGER_CHECK_LITERAL(1000);
	RANGED_INTEGER_CHECK_LITERAL(4294967295);
	RANGED_INTEGER_CHECK_LITERAL(4294967296);
	RANGED_INTEGER_CHECK_LITERAL(9223372036854775807);
	RANGED_INTEGER_CHECK_LITERAL(-1);
	RANGED_INTEGER_CHECK_LITERAL(-0);
	#undef RANGED_INTEGER_CHECK_LITERAL
}

void check_array() {
	constexpr auto dynamic_int_array = make_ranged_array(0, 3, 6);
	static_assert(dynamic_int_array.size() == 3, "Array size wrong.");
	static_assert(dynamic_int_array[2] == 6, "Array element wrong.");
	static_assert(std::is_same<decltype(make_ranged(0)), typename std::decay<decltype(dynamic_int_array[0])>::type>::value, "Array element type wrong for all int arguments.");
	
	constexpr auto dynamic_mixed_array = make_ranged_array(-6, 15u);
	static_assert(dynamic_mixed_array.size() == 2, "Array size wrong.");
	static_assert(dynamic_mixed_array[0] == -6, "Array element wrong.");
	static_assert(std::is_same<common_type_t<decltype(make_ranged(-6)), decltype(make_ranged(15u))>, typename std::decay<decltype(dynamic_mixed_array[0])>::type>::value, "Array element type wrong for mixed int / unsigned arguments.");

	constexpr auto exact_array = make_ranged_array<-100, 5, 378, 23, 10000>();
	static_assert(exact_array.size() == 5, "Array size wrong.");
	static_assert(exact_array[2] == 378, "Array element wrong.");
	static_assert(std::is_same<ranged_integer<-100, 10000, null_policy>, typename std::decay<decltype(exact_array[0])>::type>::value, "Array element type wrong for exact template arguments.");
}

template<typename integer>
void streaming_test(int const initial, int const final) {
	integer value(initial);
	std::stringstream in;
	in << value;
	assert(in.str() == std::to_string(initial));
	std::stringstream out;
	out << final;
	out >> value;
	assert(value == final);
}

void check_streaming() {
	streaming_test<checked_integer<0, 100>>(7, 0);
	constexpr auto large_initial = std::numeric_limits<int>::max() / 3;
	constexpr auto large_final = -49;
	streaming_test<decltype(make_ranged(int{}))>(large_initial, large_final);
}


}	// namespace

int main() {
	check_comparison();
	check_arithmetic();
	check_compound_arithmetic();
	check_math();
	check_numeric_limits_all();
	check_common_type();
	check_policies();
	check_array();
	check_streaming();
}
