// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "class.hpp"
#include "arithmetic/multiplies.hpp"
#include "arithmetic/plus.hpp"

namespace bounded {
namespace detail {

constexpr inline auto power(intmax_t const radix, intmax_t const exponent) noexcept -> intmax_t {
	return (exponent == 0) ? 1 : radix * power(radix, exponent - 1);
}

// I have to define value as a static member function due to a clang linker
// error. It seems that it should be usable as a static member variable instead,
// and that would simplify things a bit. It appears to be a bug in clang because
// it triggers when unrelated parts of my code change, but I am not sure.
template<char digit, char... digits>
struct literal {
private:
	static constexpr intmax_t radix = 10;
	static constexpr intmax_t integer_scale = power(radix, sizeof...(digits));
public:
	static constexpr auto value() noexcept {
		return literal<digit>::value() * constant<integer_scale> + literal<digits...>::value();
	}
};

template<char digit>
struct literal<digit> {
	static constexpr auto value() noexcept {
		return constant<digit - '0'>;
	}
};

}	// namespace detail

namespace literal {

template<char... digits>
constexpr auto operator"" _bi() noexcept {
	return detail::literal<digits...>::value();
}

}	// namespace literal

}	// namespace bounded
