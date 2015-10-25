// Specialization of numeric_limits
// Copyright (C) 2015 David Stone
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

#pragma once

#include "class.hpp"
#include <cstdint>
#include <limits>

namespace bounded {
namespace detail {

// https://stackoverflow.com/questions/19609186/what-is-stdnumeric-limitstdigits-supposed-to-represent
// digits and digits10 represent the maximum number of bits / digits in a
// lossless string -> integer -> string conversion sequence
constexpr auto test_log_successor(uintmax_t value, constant_t<2>) {
	switch (value) {
		case 1: return 1;
		case 2: return 1;
		case 3: return 2;
		case 8: return 3;
		case 9: return 3;
		case 10: return 3;
		case 11: return 3;
		case 255: return 8;
		case 998: return 9;
		case 999: return 9;
		case 1000: return 9;
		case 1022: return 9;
		case 1023: return 10;
		case 1024: return 10;
		case 1025: return 10;
		case static_cast<uint64_t>(std::numeric_limits<int64_t>::max()): return 64 - 1;
		case -static_cast<uint64_t>(std::numeric_limits<int64_t>::min()): return 64 - 1;
		// doesn't matter what we throw, compilation error
		default: throw 0;
	}
}

constexpr auto test_log_successor(uintmax_t value, constant_t<10>) {
	switch (value) {
		case 1: return 0;
		case 2: return 0;
		case 3: return 0;
		case 8: return 0;
		case 9: return 1;
		case 10: return 1;
		case 11: return 1;
		case 255: return 2;
		case 998: return 2;
		case 999: return 3;
		case 1000: return 3;
		case 1022: return 3;
		case 1023: return 3;
		case 1024: return 3;
		case 1025: return 3;
		case static_cast<uint64_t>(std::numeric_limits<int64_t>::max()): return 18;
		case -static_cast<uint64_t>(std::numeric_limits<int64_t>::min()): return 18;
		// doesn't matter what we throw, compilation error
		default: throw 0;
	}
}

template<intmax_t base>
struct digits {
private:
	static_assert(base > 1, "Base must be greater than 1.");
	static constexpr auto successor_is_power(uintmax_t value) noexcept {
		while (value != 0) {
			if (value % base != base - 1) {
				// Not evenly divisible
				return false;
			}
			value /= base;
		}
		return true;
	}
	static constexpr auto log(uintmax_t value) noexcept {
		int sum = 0;
		while (value >= base) {
			value /= base;
			++sum;
		}
		return sum;
	}
	// This must be one function rather than a chained call to three functions
	// because abs can overflow for the smallest possible value of intmax_t and
	// incrementing would overflow for either extreme.
	static constexpr auto log_successor(uintmax_t value) noexcept {
		return (value == 0) ? 0 : log(value) + (successor_is_power(value) ? 1 : 0);
	}

	#define BOUNDED_INTEGER_DIGITS_TEST(value) \
		static_assert(log_successor(value) == test_log_successor(value, constant<base>), "Incorrect digits for " #value)
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

	static_assert(log_successor(0) == 0, "Incorrect log_successor.");
public:
	static constexpr auto calculate(intmax_t minimum, intmax_t maximum) noexcept {
		if (minimum > 0 or maximum < 0) {
			return 0;
		}
		return (minimum == 0 or static_cast<uintmax_t>(maximum) < -static_cast<uintmax_t>(minimum)) ?
			log_successor(static_cast<uintmax_t>(maximum)) :
			log_successor(-static_cast<uintmax_t>(minimum));
	}
};

}	// namespace detail
}	// namespace bounded
namespace std {

template<intmax_t minimum, intmax_t maximum, typename overflow_policy, bounded::storage_type storage, bool poisoned>
struct numeric_limits<bounded::integer<minimum, maximum, overflow_policy, storage, poisoned>> {
private:
	using type = bounded::integer<minimum, maximum, overflow_policy, storage, poisoned>;
public:
	static constexpr auto is_specialized = true;
	static constexpr auto is_signed = minimum < 0;
	static constexpr auto is_integer = true;
	static constexpr auto is_exact = true;
	static constexpr auto has_infinity = false;
	static constexpr auto has_quiet_NaN = false;
	static constexpr auto has_signaling_NaN = false;
	static constexpr auto has_denorm = std::denorm_absent;
	static constexpr auto has_denorm_loss = false;
	static constexpr auto round_style = std::round_toward_zero;
	static constexpr auto is_iec559 = false;
	static constexpr auto is_bounded = true;
	static constexpr auto is_modulo = overflow_policy::is_modulo;
	static constexpr auto radix = 2;
	static constexpr auto digits = bounded::detail::digits<radix>::calculate(minimum, maximum);
	static constexpr auto digits10 = bounded::detail::digits<10>::calculate(minimum, maximum);
	static constexpr auto max_digits10 = 0;
	static constexpr auto min_exponent = 0;
	static constexpr auto min_exponent10 = 0;
	static constexpr auto max_exponent = 0;
	static constexpr auto max_exponent10 = 0;

	// If 0 is not in range, there is no trap value for arithmetic
	static constexpr auto traps =
		minimum <= 0 and 0 <= maximum and
		numeric_limits<typename type::underlying_type>::traps;

	static constexpr auto tinyness_before = false;
	
	static constexpr auto min() noexcept {
		return bounded::constant<minimum, overflow_policy, storage>;
	}
	static constexpr auto lowest() noexcept {
		return bounded::constant<minimum, overflow_policy, storage>;
	}
	static constexpr auto max() noexcept {
		return bounded::constant<maximum, overflow_policy, storage>;
	}
	// 18.3.2.7.1 says:
	// Any value that is not "meaningful" shall be set to 0 or false.
	static constexpr auto epsilon() noexcept {
		return bounded::constant<0>;
	}
	static constexpr auto round_error() noexcept {
		return bounded::constant<0>;
	}
	static constexpr auto infinity() noexcept {
		return bounded::constant<0>;
	}
	static constexpr auto quiet_NaN() noexcept {
		return bounded::constant<0>;
	}
	static constexpr auto signaling_NaN() noexcept {
		return bounded::constant<0>;
	}
	static constexpr auto denorm_min() noexcept {
		return bounded::constant<0>;
	}
};

}	// namespace std
