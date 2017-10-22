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

constexpr auto power(std::uintmax_t const radix, std::uintmax_t const exponent) noexcept {
	std::uintmax_t result = 1;
	for (std::uintmax_t n = 0; n != exponent; ++n) {
		result *= radix;
	}
	return result;
}

template<char digit, char... digits>
struct literal {
private:
	static constexpr auto radix = 10;
	static constexpr auto integer_scale = power(radix, sizeof...(digits));
public:
	static constexpr auto value = literal<digit>::value * constant<integer_scale> + literal<digits...>::value;
};

template<char digit>
struct literal<digit> {
	static constexpr auto value = constant<digit - '0'>;
};

}	// namespace detail

namespace literal {

template<char... digits>
constexpr auto operator"" _bi() noexcept {
	return detail::literal<digits...>::value;
}

}	// namespace literal

using namespace literal;

}	// namespace bounded
