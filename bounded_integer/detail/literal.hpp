// bounded type literal
// Copyright (C) 2015 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

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
