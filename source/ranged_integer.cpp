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

#include "ranged_integer.hpp"
#include <cassert>
#include <limits>
#include <iostream>

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
	RANGED_INTEGER_CHECK_FUNCTION(min);
	RANGED_INTEGER_CHECK_FUNCTION(lowest);
	RANGED_INTEGER_CHECK_FUNCTION(max);
	RANGED_INTEGER_CHECK_FUNCTION(epsilon);
	RANGED_INTEGER_CHECK_FUNCTION(round_error);
	RANGED_INTEGER_CHECK_FUNCTION(infinity);
	RANGED_INTEGER_CHECK_FUNCTION(quiet_NaN);
	RANGED_INTEGER_CHECK_FUNCTION(signaling_NaN);
	RANGED_INTEGER_CHECK_FUNCTION(denorm_min);
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

}	// namespace

int main() {
	check_numeric_limits_all();
	constexpr checked_integer<1, 10> const x(9);
	static_assert(sizeof(x) == 1, "checked_integer too big!");
	constexpr checked_integer<-3, 11> const y(x);
	constexpr checked_integer<-3, 11> const z(4);

	constexpr auto sum = x + z;
	static_assert(std::numeric_limits<decltype(sum)>::min() == -2, "Minimum sum incorrect.");
	static_assert(std::numeric_limits<decltype(sum)>::max() == 21, "Maximum sum incorrect.");
	assert(sum.value() == 13);

	constexpr auto difference = x - z;
	static_assert(std::numeric_limits<decltype(difference)>::min() == -10, "Minimum difference incorrect.");
	static_assert(std::numeric_limits<decltype(difference)>::max() == 13, "Maximum difference incorrect.");
	assert(difference.value() == 5);

	constexpr auto product = x * z;
	static_assert(std::numeric_limits<decltype(product)>::min() == -30, "Minimum product incorrect.");
	static_assert(std::numeric_limits<decltype(product)>::max() == 110, "Maximum product incorrect.");
	assert(product.value() == 36);

	constexpr auto quotient = x / z;
	static_assert(std::numeric_limits<decltype(quotient)>::min() == -10, "Minimum quotient incorrect.");
	static_assert(std::numeric_limits<decltype(quotient)>::max() == 10, "Maximum quotient incorrect.");
	assert(quotient.value() == 2);
	
	assert(quotient < product);
	assert(difference + 8 == sum);

	// constexpr checked_integer<2, 8> const z(x);
	// checked_integer<13, 63> const non_overlapping(x);
}
