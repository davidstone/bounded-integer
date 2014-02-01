// Specialization of numeric_limits
// Copyright (C) 2014 David Stone
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

#ifndef BOUNDED_INTEGER_NUMERIC_LIMITS_HPP_
#define BOUNDED_INTEGER_NUMERIC_LIMITS_HPP_

#include "forward_declaration.hpp"
#include <cstdint>
#include <limits>

namespace bounded_integer {
namespace detail {

// http://stackoverflow.com/questions/19609186/what-is-stdnumeric-limitstdigits-supposed-to-represent
template<intmax_t base>
constexpr int test_log_successor_abs(intmax_t value);
template<>
constexpr int test_log_successor_abs<2>(intmax_t value) {
	return
		(value == 1) ? 1 :
		(value == 2) ? 1 :
		(value == 3) ? 2 :
		(value == 8) ? 3 :
		(value == 9) ? 3 :
		(value == 10) ? 3 :
		(value == 11) ? 3 :
		(value == 255) ? 8 :
		(value == 998) ? 9 :
		(value == 999) ? 9 :
		(value == 1000) ? 9 :
		(value == 1022) ? 9 :
		(value == 1023) ? 10 :
		(value == 1024) ? 10 :
		(value == 1025) ? 10 :
		(value == std::numeric_limits<int64_t>::max()) ? 64 - 1 :
		(value == std::numeric_limits<int64_t>::min()) ? 64 - 1 :
		// doesn't matter what we throw, compilation error
		throw 0;
}
template<>
constexpr int test_log_successor_abs<10>(intmax_t value) {
	return
		(value == 1) ? 0 :
		(value == 2) ? 0 :
		(value == 3) ? 0 :
		(value == 8) ? 0 :
		(value == 9) ? 1 :
		(value == 10) ? 1 :
		(value == 11) ? 1 :
		(value == 255) ? 2 :
		(value == 998) ? 2 :
		(value == 999) ? 3 :
		(value == 1000) ? 3 :
		(value == 1022) ? 3 :
		(value == 1023) ? 3 :
		(value == 1024) ? 3 :
		(value == 1025) ? 3 :
		(value == std::numeric_limits<int64_t>::max()) ? 18 :
		(value == std::numeric_limits<int64_t>::min()) ? 18 :
		// doesn't matter what we throw, compilation error
		throw 0;
}

template<intmax_t base>
class digits {
private:
	// I don't know if this is actually correct. I could theoretically use a
	// more compact representation of values that have a minimum that is far
	// from zero. For instance, it only takes a single bit to represent the
	// value in the range of 500 through 501, but my current implementation
	// would actually use 9 bits of space. I don't know what this value is
	// ultimately supposed to represent, and I may create a specialized version
	// to compact the representation.

	static_assert(base > 1, "Base must be greater than 1.");
	static constexpr bool successor_is_power(intmax_t value) noexcept {
		return
			(value == 0) ? true :
			(value % base != base - 1 and value % base != -base + 1) ? false :
			successor_is_power(value / base);
	}
	static constexpr int log_abs(intmax_t value, int sum) noexcept {
		return (-base < value and value < base) ? sum : log_abs(value / base, sum + 1);
	}
	// This must be one function rather than a chained call to three functions
	// because abs can overflow for the smallest possible value of intmax_t and
	// incrementing would overflow for either extreme.
	static constexpr int log_successor_abs(intmax_t value) noexcept {
		return (value == 0) ? 0 : log_abs(value, 0) + (successor_is_power(value) ? 1 : 0);
	}

	#define BOUNDED_INTEGER_DIGITS_TEST(value) \
		static_assert(log_successor_abs(value) == test_log_successor_abs<base>(value), "Incorrect digits for " #value)
	BOUNDED_INTEGER_DIGITS_TEST(1);
	BOUNDED_INTEGER_DIGITS_TEST(2);
	BOUNDED_INTEGER_DIGITS_TEST(3);
	BOUNDED_INTEGER_DIGITS_TEST(8);
	BOUNDED_INTEGER_DIGITS_TEST(9);
	BOUNDED_INTEGER_DIGITS_TEST(10);
	BOUNDED_INTEGER_DIGITS_TEST(11);
	BOUNDED_INTEGER_DIGITS_TEST(255);
	BOUNDED_INTEGER_DIGITS_TEST(998);
	BOUNDED_INTEGER_DIGITS_TEST(999);
	BOUNDED_INTEGER_DIGITS_TEST(1000);
	BOUNDED_INTEGER_DIGITS_TEST(1022);
	BOUNDED_INTEGER_DIGITS_TEST(1023);
	BOUNDED_INTEGER_DIGITS_TEST(1024);
	BOUNDED_INTEGER_DIGITS_TEST(1025);
	BOUNDED_INTEGER_DIGITS_TEST(std::numeric_limits<int64_t>::max());
	BOUNDED_INTEGER_DIGITS_TEST(std::numeric_limits<int64_t>::min());
	#undef BOUNDED_INTEGER_DIGITS_TEST

	#define BOUNDED_INTEGER_NEGATIVE_DIGITS_TEST(value) \
		static_assert(log_successor_abs(value) == log_successor_abs(-value), "Incorrect digits for -" #value)
	BOUNDED_INTEGER_NEGATIVE_DIGITS_TEST(1);
	BOUNDED_INTEGER_NEGATIVE_DIGITS_TEST(2);
	BOUNDED_INTEGER_NEGATIVE_DIGITS_TEST(3);
	BOUNDED_INTEGER_NEGATIVE_DIGITS_TEST(8);
	BOUNDED_INTEGER_NEGATIVE_DIGITS_TEST(9);
	BOUNDED_INTEGER_NEGATIVE_DIGITS_TEST(10);
	BOUNDED_INTEGER_NEGATIVE_DIGITS_TEST(11);
	BOUNDED_INTEGER_NEGATIVE_DIGITS_TEST(1000);
	BOUNDED_INTEGER_NEGATIVE_DIGITS_TEST(1023);
	BOUNDED_INTEGER_NEGATIVE_DIGITS_TEST(1024);
	BOUNDED_INTEGER_NEGATIVE_DIGITS_TEST(1025);
	#undef BOUNDED_INTEGER_NEGATIVE_DIGITS_TEST

	static_assert(log_successor_abs(0) == 0, "Incorrect abs.");
public:
	static constexpr int calculate(intmax_t minimum, intmax_t maximum) noexcept {
		return
			(0 < minimum or maximum < 0) ?
				0 :
				(minimum == 0 or log_successor_abs(maximum) < log_successor_abs(minimum)) ?
					log_successor_abs(maximum) :
					log_successor_abs(minimum);
	}
};

}	// namespace detail
}	// namespace bounded_integer
namespace std {

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounded_integer::bounds bound>
class numeric_limits<bounded_integer::bounded_integer<minimum, maximum, overflow_policy, bound>> {
private:
	using type = bounded_integer::bounded_integer<minimum, maximum, overflow_policy, bound>;
public:
	static constexpr bool is_specialized = true;
	static constexpr bool is_signed = minimum < 0;
	static constexpr bool is_integer = true;
	static constexpr bool is_exact = true;
	static constexpr bool has_infinity = false;
	static constexpr bool has_quiet_NaN = false;
	static constexpr bool has_signaling_NaN = false;
	static constexpr auto has_denorm = std::denorm_absent;
	static constexpr bool has_denorm_loss = false;
	static constexpr auto round_style = std::round_toward_zero;
	static constexpr bool is_iec559 = false;
	static constexpr bool is_bounded = true;
	static constexpr bool is_modulo = overflow_policy::is_modulo;
	static constexpr int radix = 2;
	static constexpr int digits = bounded_integer::detail::digits<radix>::calculate(minimum, maximum);
	static constexpr int digits10 = bounded_integer::detail::digits<10>::calculate(minimum, maximum);
	static constexpr int max_digits10 = 0;
	static constexpr int min_exponent = 0;
	static constexpr int min_exponent10 = 0;
	static constexpr int max_exponent = 0;
	static constexpr int max_exponent10 = 0;
	// If 0 is not in range, there is no trap value for arithmetic
	static constexpr bool traps = minimum <= 0 and 0 <= maximum;
	static constexpr bool tinyness_before = false;
	
	static constexpr type min() noexcept {
		return type(minimum, bounded_integer::non_check);
	}
	static constexpr type lowest() noexcept {
		return type(minimum, bounded_integer::non_check);
	}
	static constexpr type max() noexcept {
		return type(maximum, bounded_integer::non_check);
	}
	// Some of these functions return 0 for built-in integer types, but 0 may
	// not be in the representable range. Fortunately, they are also defined as
	// only being meaningful for floating-point types. Unfortunately, 18.3.2.7.1
	// says: Any value that is not "meaningful" shall be set to 0 or false. This
	// therefore technically invokes undefined behavior because 0 might not be
	// in the range of representable values
	static constexpr type epsilon() noexcept {
		return type(minimum, bounded_integer::non_check);
	}
	static constexpr type round_error() noexcept {
		return type(minimum, bounded_integer::non_check);
	}
	static constexpr type infinity() noexcept {
		return type(minimum, bounded_integer::non_check);
	}
	static constexpr type quiet_NaN() noexcept {
		return type(minimum, bounded_integer::non_check);
	}
	static constexpr type signaling_NaN() noexcept {
		return type(minimum, bounded_integer::non_check);
	}
	static constexpr type denorm_min() noexcept {
		return type(minimum, bounded_integer::non_check);
	}
};

}	// namespace std
#endif	// BOUNDED_INTEGER_NUMERIC_LIMITS_HPP_
