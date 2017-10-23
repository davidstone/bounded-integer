// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/arithmetic/multiplies.hpp>
#include <bounded/detail/arithmetic/plus.hpp>

namespace bounded {
namespace detail {

constexpr auto power(std::uintmax_t const radix, std::uintmax_t const exponent) {
	std::uintmax_t result = 1;
	for (std::uintmax_t n = 0; n != exponent; ++n) {
		result *= radix;
	}
	return result;
}

template<typename Digit>
constexpr auto literal(Digit const digit) {
	return digit;
}
template<typename Digit, typename... Digits>
constexpr auto literal(Digit const digit, Digits... digits) {
	constexpr auto radix = 10;
	return digit * constant<power(radix, sizeof...(digits))> + literal(digits...);
}

}	// namespace detail

namespace literal {

template<char... digits>
constexpr auto operator""_bi() noexcept {
	return detail::literal(constant<digits - '0'>...);
}

}	// namespace literal

using namespace literal;

}	// namespace bounded
