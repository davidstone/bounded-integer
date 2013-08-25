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

#include "class.hpp"
#include "operators.hpp"
#include "policy.hpp"

namespace detail {

template<intmax_t minimum, intmax_t maximum>
using literal_policy = null_policy<minimum, maximum>;

template<intmax_t value>
using ranged_constant = ranged_integer<value, value, literal_policy>;

constexpr intmax_t power(intmax_t const radix, intmax_t const exponent) {
	return (exponent == 0) ? 1 : radix * power(radix, exponent - 1);
}

template<char digit, char... digits>
class literal_ranged_integer {
private:
	static constexpr intmax_t radix = 10;
	static constexpr intmax_t integer_scale = power(radix, sizeof...(digits));
	static constexpr ranged_constant<integer_scale> scale{};
public:
	static constexpr decltype(literal_ranged_integer<digit>::value() * scale + literal_ranged_integer<digits...>::value()) value() {
		return literal_ranged_integer<digit>::value() * scale + literal_ranged_integer<digits...>::value();
	}
};

template<char digit>
class literal_ranged_integer<digit> {
private:
	using result_type = ranged_constant<digit - '0'>;
public:
	static constexpr result_type value() {
		return result_type{};
	}
};

}	// namespace detail

template<char... digits>
constexpr decltype(detail::literal_ranged_integer<digits...>::value()) operator"" _ranged_integer() {
	return detail::literal_ranged_integer<digits...>::value();
}

#endif	// RANGED_INTEGER_LITERAL_HPP_
