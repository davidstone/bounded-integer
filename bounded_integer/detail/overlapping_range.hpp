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

#include "enable_if.hpp"
#include "forward_declaration.hpp"
#include "numeric_limits.hpp"
#include <cstdint>
#include <type_traits>

namespace bounded_integer {
namespace detail {

template<typename integer, enable_if_t<std::numeric_limits<integer>::is_specialized> = clang_dummy>
constexpr bool entirely_in_range(intmax_t const minimum, intmax_t const maximum) noexcept {
	return
		static_cast<intmax_t>(std::numeric_limits<integer>::min()) <= minimum and
		maximum <= static_cast<intmax_t>(std::numeric_limits<integer>::max());
}
template<typename integer, enable_if_t<!std::numeric_limits<integer>::is_specialized> = clang_dummy>
constexpr bool entirely_in_range(intmax_t, intmax_t) noexcept {
	return false;
}
template<>
constexpr bool entirely_in_range<uintmax_t>(intmax_t const minimum, intmax_t) noexcept {
	return minimum >= 0;
}

template<typename integer, enable_if_t<std::numeric_limits<integer>::is_specialized> = clang_dummy>
constexpr bool has_overlap(intmax_t const minimum, intmax_t const maximum) noexcept {
	return
		minimum <= static_cast<intmax_t>(std::numeric_limits<integer>::max()) and
		maximum >= static_cast<intmax_t>(std::numeric_limits<integer>::min());
}
template<typename integer, enable_if_t<!std::numeric_limits<integer>::is_specialized> = clang_dummy>
constexpr bool has_overlap(intmax_t, intmax_t) noexcept {
	return false;
}
template<>
constexpr bool has_overlap<uintmax_t>(intmax_t, intmax_t const maximum) noexcept {
	return maximum >= 0;
}

template<typename integer, enable_if_t<std::numeric_limits<integer>::is_specialized> = clang_dummy>
constexpr bool type_in_range(intmax_t const minimum, intmax_t const maximum) noexcept {
	return
		minimum <= static_cast<intmax_t>(std::numeric_limits<integer>::min()) and
		static_cast<intmax_t>(std::numeric_limits<integer>::max()) <= maximum;
}
template<typename integer, enable_if_t<!std::numeric_limits<integer>::is_specialized> = clang_dummy>
constexpr bool type_in_range(intmax_t, intmax_t) noexcept {
	return false;
}
template<>
constexpr bool type_in_range<uintmax_t>(intmax_t const, intmax_t const) noexcept {
	return false;
}

}	// namespace detail
}	// namespace bounded_integer
#endif	// BOUNDED_INTEGER_OVERLAPPING_RANGE_HPP_
