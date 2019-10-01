// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/assert.hpp>
#include <bounded/detail/arithmetic/multiplies.hpp>
#include <bounded/detail/arithmetic/plus.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/max_builtin.hpp>

namespace bounded {
namespace detail {

template<char... digits>
constexpr auto literal_impl() {
	max_unsigned_t result = 0;
	auto accumulate_digit = [&](char const digit) {
		if (digit == '\'') {
			return;
		}
		constexpr auto radix = 10;
		result *= radix;
		auto const digit_num = static_cast<max_unsigned_t>(digit - '0');
		BOUNDED_ASSERT(digit_num <= 9);
		result += digit_num;
	};
	(..., accumulate_digit(digits));
	return result;
}

}	// namespace detail

namespace literal {

template<char... digits>
constexpr auto operator""_bi() {
	return constant<::bounded::detail::literal_impl<digits...>()>;
}

}	// namespace literal

using namespace literal;

}	// namespace bounded
