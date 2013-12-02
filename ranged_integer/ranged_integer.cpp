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

#include "optional.hpp"
#include "ranged_integer.hpp"
#include <cassert>
#include <sstream>

namespace {

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
	x %= 6_ri;
	assert(x == 4);
	
	short s = 0;
	s += 4_ri;
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
	i %= 4_ri;
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

void check_throw_policy() {
	static constexpr intmax_t minimum = 0;
	static constexpr intmax_t maximum = 10;
	throw_policy policy;
	try {
		policy.assignment<minimum, maximum>(20);
		assert(false);
	}
	catch (std::overflow_error const &) {
	}
	try {
		policy.assignment<minimum, maximum>(-6);
		assert(false);
	}
	catch (std::underflow_error const &) {
	}
}

void check_policies() {
	check_throw_policy();
}

template<typename T>
void check_uncompressed_optional() {
	using type = ranged_integer<std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), null_policy>;
	static_assert(sizeof(type) < sizeof(optional<type>), "Compressing an optional that should not be compressed.");
}
template<intmax_t minimum, intmax_t maximum>
void check_compressed_optional() {
	using type = ranged_integer<minimum, maximum, null_policy>;
	using compressed_type = optional<type>;
	static_assert(sizeof(type) == sizeof(compressed_type), "compressed_optional too big.");
	static_assert(sizeof(compressed_type) == sizeof(optional<type>), "Incorrect optional selection.");
}

void check_optional() {
	check_compressed_optional<1, 10>();
	check_compressed_optional<-50, 127>();
	check_uncompressed_optional<uint8_t>();
	check_uncompressed_optional<int>();
	check_uncompressed_optional<unsigned>();
	check_uncompressed_optional<intmax_t>();
	
	using ri_type = checked_integer<1, 10>;
	constexpr optional<ri_type> uninitialized_optional;
	static_assert(!uninitialized_optional, "Default constructor should leave uninitialized.");
	constexpr optional<ri_type> constexpr_optional_integer(ri_type(5));
	static_assert(static_cast<bool>(constexpr_optional_integer), "Value constructor should initialize optional.");
	static_assert(*constexpr_optional_integer == 5, "Value in an optional incorrect.");
	optional<ri_type> optional_integer(ri_type(4));
	optional_integer = uninitialized_optional;
	assert(!optional_integer);
	optional_integer = ri_type(7);
	assert(optional_integer);
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
	streaming_test<equivalent_type<int>>(large_initial, large_final);
}


}	// namespace

int main() {
	check_compound_arithmetic();
	check_math();
	check_numeric_limits_all();
	check_policies();
	check_optional();
	check_streaming();
}
