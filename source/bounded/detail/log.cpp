// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/log.hpp>
#include <bounded/detail/arithmetic/common.hpp>
#include <bounded/detail/comparison_builtin.hpp>
#include <bounded/detail/class.hpp>

namespace {

template<typename Integer>
constexpr auto test_log(Integer const value, bounded::constant_t<2>) {
	switch (static_cast<bounded::detail::max_unsigned_t>(value)) {
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
		case std::numeric_limits<std::int64_t>::max(): return 62;
		case std::numeric_limits<std::uint64_t>::max(): return 63;
#if defined BOUNDED_DETAIL_HAS_128_BIT
		case bounded::basic_numeric_limits<bounded::detail::int128_t>::max(): return 126;
		case bounded::basic_numeric_limits<bounded::detail::uint128_t>::max(): return 127;
#endif
		// doesn't matter what we throw, compilation error
		default: throw 0;
	}
}

template<typename Integer>
constexpr auto test_log(Integer const value, bounded::constant_t<10>) {
	switch (static_cast<bounded::detail::max_unsigned_t>(value)) {
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
		case std::numeric_limits<std::int64_t>::max(): return 18;
		case std::numeric_limits<std::uint64_t>::max(): return 19;
#if defined BOUNDED_DETAIL_HAS_128_BIT
		case bounded::basic_numeric_limits<bounded::detail::int128_t>::max(): return 38;
		case bounded::basic_numeric_limits<bounded::detail::uint128_t>::max(): return 38;
#endif
		// doesn't matter what we throw, compilation error
		default: throw 0;
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
BOUNDED_INTEGER_LOG_TEST(std::numeric_limits<std::int64_t>::max());
BOUNDED_INTEGER_LOG_TEST(std::numeric_limits<std::uint64_t>::max());
#if defined BOUNDED_DETAIL_HAS_128_BIT
BOUNDED_INTEGER_LOG_TEST(bounded::basic_numeric_limits<bounded::detail::int128_t>::max());
BOUNDED_INTEGER_LOG_TEST(bounded::basic_numeric_limits<bounded::detail::uint128_t>::max());
#endif

#undef BOUNDED_INTEGER_LOG_TEST
#undef BOUNDED_INTEGER_LOG_TEST_INDIVIDUAL

}	// namespace
