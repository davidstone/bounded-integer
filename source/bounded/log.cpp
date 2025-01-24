// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <numeric_traits/has_int128.hpp>

export module bounded.log;

import bounded.arithmetic.operators;
import bounded.bounded_integer;
import bounded.builtin_min_max_value;
import bounded.comparison;
import bounded.comparison_builtin;
import bounded.integer;
import bounded.unchecked;

import numeric_traits;
import std_module;

namespace bounded {

constexpr auto log_impl(auto value, auto const base) -> int {
	auto sum = 0;
	while (value >= base) {
		value /= base;
		++sum;
	}
	return sum;
}

constexpr auto as_unsigned(auto const value) {
	return static_cast<numeric_traits::make_unsigned<decltype(value)>>(value);
};

template<typename T>
concept supports_bit_width = requires(T integer) {
	std::bit_width(as_unsigned(integer.value()));
};

// TODO: It's useful for 0 to return 0, but that's not `log`.
export template<bounded_integer Value, bounded_integer Base>
constexpr auto log(Value const value, Base const base) {
	static_assert(base > constant<1>, "Negative bases not currently supported.");
	static_assert(numeric_traits::min_value<Value> >= constant<0>, "The log of a negative number is undefined.");
	using result_type = integer<
		log_impl(static_cast<numeric_traits::max_unsigned_t>(builtin_min_value<Value>), static_cast<numeric_traits::max_unsigned_t>(numeric_traits::max_value<Base>)),
		log_impl(static_cast<numeric_traits::max_unsigned_t>(builtin_max_value<Value>), static_cast<numeric_traits::max_unsigned_t>(numeric_traits::min_value<Base>))
	>;
	if constexpr (base == constant<2> and supports_bit_width<Value>) {
		return result_type(value == constant<0> ? 0 : std::bit_width(as_unsigned(value.value())) - 1, unchecked);
	} else {
		return result_type(log_impl(as_unsigned(value.value()), as_unsigned(base.value())), unchecked);
	}
}

} // namespace bounded

constexpr auto test_log(auto const value, bounded::constant_t<2>) {
	switch (static_cast<numeric_traits::max_unsigned_t>(value)) {
		case 1: return 0;
		case 2: return 1;
		case 3: return 1;
		case 8: return 3;
		case 9: return 3;
		case 10: return 3;
		case 11: return 3;
		case 255: return 7;
		case 998: return 9;
		case 999: return 9;
		case 1000: return 9;
		case 1022: return 9;
		case 1023: return 9;
		case 1024: return 10;
		case 1025: return 10;
		case numeric_traits::max_value<std::int64_t>: return 62;
		case numeric_traits::max_value<std::uint64_t>: return 63;
#if defined NUMERIC_TRAITS_HAS_INT128
		case numeric_traits::max_value<numeric_traits::int128_t>: return 126;
		case numeric_traits::max_value<numeric_traits::uint128_t>: return 127;
#endif
		default: std::unreachable();
	}
}

constexpr auto test_log(auto const value, bounded::constant_t<10>) {
	switch (static_cast<numeric_traits::max_unsigned_t>(value)) {
		case 1: return 0;
		case 2: return 0;
		case 3: return 0;
		case 8: return 0;
		case 9: return 0;
		case 10: return 1;
		case 11: return 1;
		case 255: return 2;
		case 998: return 2;
		case 999: return 2;
		case 1000: return 3;
		case 1022: return 3;
		case 1023: return 3;
		case 1024: return 3;
		case 1025: return 3;
		case numeric_traits::max_value<std::int64_t>: return 18;
		case numeric_traits::max_value<std::uint64_t>: return 19;
#if defined NUMERIC_TRAITS_HAS_INT128
		case numeric_traits::max_value<numeric_traits::int128_t>: return 38;
		case numeric_traits::max_value<numeric_traits::uint128_t>: return 38;
#endif
		default: std::unreachable();
	}
}

#define BOUNDED_INTEGER_LOG_TEST_INDIVIDUAL(value, base) \
	static_assert( \
		bounded::log(bounded::constant<value>, bounded::constant<base>) == test_log(value, bounded::constant<base>), \
		"Incorrect log for " #value " in base " #base \
	)

#define BOUNDED_INTEGER_LOG_TEST(value) \
	BOUNDED_INTEGER_LOG_TEST_INDIVIDUAL(value, 2); \
	BOUNDED_INTEGER_LOG_TEST_INDIVIDUAL(value, 10)

BOUNDED_INTEGER_LOG_TEST(1);
BOUNDED_INTEGER_LOG_TEST(2);
BOUNDED_INTEGER_LOG_TEST(3);
BOUNDED_INTEGER_LOG_TEST(8);
BOUNDED_INTEGER_LOG_TEST(9);
BOUNDED_INTEGER_LOG_TEST(10);
BOUNDED_INTEGER_LOG_TEST(11);
BOUNDED_INTEGER_LOG_TEST(255);
BOUNDED_INTEGER_LOG_TEST(998);
BOUNDED_INTEGER_LOG_TEST(999);
BOUNDED_INTEGER_LOG_TEST(1000);
BOUNDED_INTEGER_LOG_TEST(1022);
BOUNDED_INTEGER_LOG_TEST(1023);
BOUNDED_INTEGER_LOG_TEST(1024);
BOUNDED_INTEGER_LOG_TEST(1025);
BOUNDED_INTEGER_LOG_TEST(numeric_traits::max_value<std::int64_t>);
BOUNDED_INTEGER_LOG_TEST(numeric_traits::max_value<std::uint64_t>);
#if defined NUMERIC_TRAITS_HAS_INT128
BOUNDED_INTEGER_LOG_TEST(numeric_traits::max_value<numeric_traits::int128_t>);
BOUNDED_INTEGER_LOG_TEST(numeric_traits::max_value<numeric_traits::uint128_t>);
#endif