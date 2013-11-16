// Ranged integer type literal
// Copyright (C) 2013 David Stone
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

#ifndef RANGED_INTEGER_LITERAL_HPP_
#define RANGED_INTEGER_LITERAL_HPP_

#include "arithmetic_operators.hpp"
#include "class.hpp"
#include "make_ranged.hpp"
#include "policy.hpp"

namespace detail {

constexpr inline intmax_t power(intmax_t const radix, intmax_t const exponent) noexcept {
	return (exponent == 0) ? 1 : radix * power(radix, exponent - 1);
}

template<char digit, char... digits>
class literal_ranged_integer {
private:
	static constexpr intmax_t radix = 10;
	static constexpr intmax_t integer_scale = power(radix, sizeof...(digits));
public:
	static constexpr auto value = literal_ranged_integer<digit>::value * make_ranged<integer_scale>() + literal_ranged_integer<digits...>::value;
};

template<char digit>
class literal_ranged_integer<digit> {
public:
	static constexpr auto value = make_ranged<digit - '0'>();
};

}	// namespace detail

template<char... digits>
constexpr decltype(detail::literal_ranged_integer<digits...>::value) operator"" _ri() noexcept {
	return detail::literal_ranged_integer<digits...>::value;
}

#endif	// RANGED_INTEGER_LITERAL_HPP_


