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
    static_cast<unsigned>(bounded::max_value<int>),
    bounded::max_value<int>
));
static_assert(check_value_and_type(
    bounded::max_value<unsigned char>,
    static_cast<signed char>(-1)
));
static_assert(check_value_and_type(
    bounded::max_value<unsigned short>,
    static_cast<short>(-1)
));
static_assert(check_value_and_type(
    bounded::max_value<unsigned>,
    static_cast<int>(-1)
));
static_assert(check_value_and_type(
    bounded::max_value<unsigned long>,
    static_cast<long>(-1)
));
static_assert(check_value_and_type(
    bounded::max_value<unsigned long long>,
    static_cast<long long>(-1)
));

using bounded::detail::max_unsigned_t;
using bounded::detail::max_signed_t;

static_assert(check_value_and_type(
    bounded::max_value<max_unsigned_t>,
    static_cast<max_signed_t>(-1)
));
static_assert(check_value_and_type(
    static_cast<max_unsigned_t>(bounded::max_value<max_signed_t>),
    static_cast<max_signed_t>(bounded::max_value<max_signed_t>)
));
static_assert(check_value_and_type(
    static_cast<max_unsigned_t>(bounded::min_value<max_signed_t>),
    static_cast<max_signed_t>(bounded::min_value<max_signed_t>)
));

static_assert(check_value_and_type(
    bounded::max_value<unsigned> - 12,
    static_cast<int>(-13)
));

static_assert(check_value_and_type(
    76U,
    76
));

} // namespace