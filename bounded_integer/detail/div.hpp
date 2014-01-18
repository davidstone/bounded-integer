// Overloads the standard library function that returns quotient and remainder
// Copyright (C) 2014 David Stone
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

#ifndef BOUNDED_INTEGER_DIV_HPP_
#define BOUNDED_INTEGER_DIV_HPP_

#include <utility>

namespace bounded_integer {

template<typename Numerator, typename Denominator>
struct div_t {
	decltype(std::declval<Numerator>() / std::declval<Denominator>()) quot;
	decltype(std::declval<Numerator>() % std::declval<Denominator>()) rem;
};

// Compiler optimizations can make this use a single instruction. See
// https://stackoverflow.com/questions/7070346/c-best-way-to-get-integer-division-and-remainder
template<typename Numerator, typename Denominator>
constexpr auto div(Numerator const numerator, Denominator const denominator) noexcept {
	return div_t<Numerator, Denominator>{ numerator / denominator, numerator % denominator };
}

}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_DIV_HPP_
