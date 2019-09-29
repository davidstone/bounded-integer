// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/modulo_cast.hpp>

namespace {

constexpr auto check_value_and_type(auto const input, auto const expected) {
    auto const result = bounded::detail::cast_to_signed_integer(input);
	static_assert(std::is_same_v<decltype(result), decltype(expected)>);
	return result == expected;
}

static_assert(check_value_and_type(
    0U,
    0
));
static_assert(check_value_and_type(
    1U,
    1
));
static_assert(check_value_and_type(
    -1U,
    -1
));
static_assert(check_value_and_type(
    static_cast<unsigned>(std::numeric_limits<int>::max()),
    std::numeric_limits<int>::max()
));
static_assert(check_value_and_type(
    std::numeric_limits<unsigned char>::max(),
    static_cast<signed char>(-1)
));
static_assert(check_value_and_type(
    std::numeric_limits<unsigned short>::max(),
    static_cast<short>(-1)
));
static_assert(check_value_and_type(
    std::numeric_limits<unsigned>::max(),
    static_cast<int>(-1)
));
static_assert(check_value_and_type(
    std::numeric_limits<unsigned long>::max(),
    static_cast<long>(-1)
));
static_assert(check_value_and_type(
    std::numeric_limits<unsigned long long>::max(),
    static_cast<long long>(-1)
));

using bounded::detail::max_unsigned_t;
using bounded::detail::max_signed_t;

static_assert(check_value_and_type(
    bounded::basic_numeric_limits<max_unsigned_t>::max(),
    static_cast<max_signed_t>(-1)
));
static_assert(check_value_and_type(
    static_cast<max_unsigned_t>(bounded::basic_numeric_limits<max_signed_t>::max()),
    static_cast<max_signed_t>(bounded::basic_numeric_limits<max_signed_t>::max())
));
static_assert(check_value_and_type(
    static_cast<max_unsigned_t>(bounded::basic_numeric_limits<max_signed_t>::min()),
    static_cast<max_signed_t>(bounded::basic_numeric_limits<max_signed_t>::min())
));

static_assert(check_value_and_type(
    std::numeric_limits<unsigned>::max() - 12,
    static_cast<int>(-13)
));

static_assert(check_value_and_type(
    76U,
    76
));

} // namespace