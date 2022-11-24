// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/assert.hpp>
#include <bounded/detail/arithmetic/multiplies.hpp>
#include <bounded/detail/arithmetic/plus.hpp>
#include <bounded/detail/class.hpp>

#include <numeric_traits/int128.hpp>

#include <cstddef>

namespace bounded {
namespace detail {

template<typename T, std::size_t size>
using c_array = T[size];

template<std::size_t size>
consteval auto literal_impl(c_array<char, size> && digits) {
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

} // namespace detail

namespace literal {

template<char... digits>
consteval auto operator""_bi() {
	return constant<::bounded::detail::literal_impl({digits...})>;
}

} // namespace literal

using namespace literal;

} // namespace bounded
