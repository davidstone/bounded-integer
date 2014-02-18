// Determine to what extent a type's range lies within another range
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

#ifndef BOUNDED_INTEGER_OVERLAPPING_RANGE_HPP_
#define BOUNDED_INTEGER_OVERLAPPING_RANGE_HPP_

#include "forward_declaration.hpp"
#include <cstdint>
#include <limits>
#include <type_traits>

// All of these functions have a precondition that minimum <= maximum

namespace bounded_integer {
namespace detail {

template<typename integer>
class basic_numeric_limits {
public:
	static constexpr intmax_t min() noexcept {
		return std::numeric_limits<integer>::min();
	}
	static constexpr intmax_t max() noexcept {
		return std::numeric_limits<integer>::max();
	}
	static constexpr bool is_specialized = std::numeric_limits<integer>::is_specialized;
};
template<intmax_t minimum, intmax_t maximum, typename overflow_policy>
class basic_numeric_limits<bounded_integer<minimum, maximum, overflow_policy>> {
public:
	static constexpr intmax_t min() noexcept {
		return minimum;
	}
	static constexpr intmax_t max() noexcept {
		return maximum;
	}
	static constexpr bool is_specialized = true;
};

template<typename integer>
constexpr bool value_fits_in_type(intmax_t const value) noexcept {
	static_assert(basic_numeric_limits<integer>::is_specialized, "Only works with integer types.");
	return basic_numeric_limits<integer>::min() <= value and value <= basic_numeric_limits<integer>::max();
}
template<>
constexpr bool value_fits_in_type<uintmax_t>(intmax_t const value) noexcept {
	return value >= 0;
}

template<typename integer>
constexpr bool type_overlaps_range(intmax_t const minimum, intmax_t const maximum) noexcept {
	static_assert(basic_numeric_limits<integer>::is_specialized, "Only works with integer types.");
	return
		minimum <= basic_numeric_limits<integer>::max() and
		basic_numeric_limits<integer>::min() <= maximum;
}
template<>
constexpr bool type_overlaps_range<uintmax_t>(intmax_t, intmax_t const maximum) noexcept {
	return maximum >= 0;
}

template<typename integer>
constexpr bool type_fits_in_range(intmax_t const minimum, intmax_t const maximum) noexcept {
	static_assert(basic_numeric_limits<integer>::is_specialized, "Only works with integer types.");
	return
		minimum <= basic_numeric_limits<integer>::min() and
		basic_numeric_limits<integer>::max() <= maximum;
}
template<>
constexpr bool type_fits_in_range<uintmax_t>(intmax_t const, intmax_t const) noexcept {
	return false;
}

}	// namespace detail
}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_OVERLAPPING_RANGE_HPP_
