// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assert.hpp>

export module bounded.literal;

import bounded.arithmetic.multiplies;
import bounded.arithmetic.plus;
import bounded.homogeneous_equals;
import bounded.integer;

import numeric_traits;
import std_module;

namespace bounded {

template<typename T, std::size_t size>
using c_array = T[size];

// consteval is broken in clang, see
// https://github.com/llvm/llvm-project/issues/90130
#ifdef __clang__
#define CONSTEVAL constexpr
#else
#define CONSTEVAL consteval
#endif

template<std::size_t size>
CONSTEVAL auto literal_impl(c_array<char, size> && digits) {
	numeric_traits::max_unsigned_t result = 0;
	for (auto const digit : digits) {
		if (digit == '\'') {
			continue;
		}
		constexpr auto radix = 10;
		result *= radix;
		auto const digit_num = static_cast<numeric_traits::max_unsigned_t>(digit - '0');
		BOUNDED_ASSERT(digit_num <= 9);
		result += digit_num;
	}
	return result;
}

inline namespace literal {

export template<char... digits>
CONSTEVAL auto operator""_bi() {
	return constant<literal_impl({digits...})>;
}

} // namespace literal
} // namespace bounded

using namespace bounded::literal;

// I have to use the preprocessor here to create an integer literal
#define BOUNDED_INTEGER_CHECK_LITERAL(x) \
	static_assert(homogeneous_equals( \
		x ## _bi, \
		bounded::constant<x> \
	))

BOUNDED_INTEGER_CHECK_LITERAL(0);
BOUNDED_INTEGER_CHECK_LITERAL(1);
BOUNDED_INTEGER_CHECK_LITERAL(10);
BOUNDED_INTEGER_CHECK_LITERAL(1000);
BOUNDED_INTEGER_CHECK_LITERAL(4294967295);
BOUNDED_INTEGER_CHECK_LITERAL(4294967296);
BOUNDED_INTEGER_CHECK_LITERAL(9223372036854775807);
