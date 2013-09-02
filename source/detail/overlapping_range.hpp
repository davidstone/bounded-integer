// Determine to what extent a type's range lies within another range
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

#ifndef RANGED_INTEGER_OVERLAPPING_RANGE_HPP_
#define RANGED_INTEGER_OVERLAPPING_RANGE_HPP_

#include "numeric_limits.hpp"
#include <cstdint>

namespace detail {

template<typename integer>
constexpr intmax_t numeric_min() noexcept {
	return static_cast<intmax_t>(std::numeric_limits<integer>::min());
}
template<typename integer>
constexpr intmax_t numeric_max() noexcept {
	return static_cast<intmax_t>(std::numeric_limits<integer>::max());
}

template<typename integer>
constexpr bool value_in_range(intmax_t const value) noexcept {
	return numeric_min<integer>() <= value and value <= numeric_max<integer>();
}
template<>
constexpr bool value_in_range<uintmax_t>(intmax_t const value) noexcept {
	return value >= 0;
}

template<typename integer>
constexpr bool has_overlap(intmax_t const minimum, intmax_t const maximum) noexcept {
	return minimum <= numeric_max<integer>() and maximum >= numeric_min<integer>();
}
template<>
constexpr bool has_overlap<uintmax_t>(intmax_t const minimum, intmax_t const maximum) noexcept {
	return maximum >= 0;
}

template<typename integer>
constexpr bool type_can_underflow(intmax_t const minimum) noexcept {
	return numeric_min<integer>() < minimum;
}
template<typename integer>
constexpr bool type_can_overflow(intmax_t const maximum) noexcept {
	return numeric_min<integer>() > maximum;
}

template<typename integer>
constexpr bool type_in_range(intmax_t const minimum, intmax_t const maximum) noexcept {
	return !type_can_underflow<integer>(minimum) and !type_can_overflow<integer>(maximum);
}

}	// namespace detail

#endif	// RANGED_INTEGER_OVERLAPPING_RANGE_HPP_
