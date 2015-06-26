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

#pragma once

#include "comparison.hpp"
#include "forward_declaration.hpp"
#include "numeric_limits.hpp"

#include <cstdint>
#include <type_traits>

// All of these functions have a precondition that minimum <= maximum

namespace bounded {
namespace detail {

template<typename T>
constexpr auto value_fits_in_type(intmax_t const value) noexcept {
	static_assert(basic_numeric_limits<T>::is_specialized, "Only works with integer types.");
	if (std::is_same<T, uintmax_t>::value) {
		return value >= 0;
	}
	return basic_numeric_limits<T>::min() <= value and value <= basic_numeric_limits<T>::max();
}

template<typename T>
constexpr auto type_overlaps_range(intmax_t const minimum, intmax_t const maximum) noexcept {
	static_assert(basic_numeric_limits<T>::is_specialized, "Only works with integer types.");
	if (std::is_same<T, uintmax_t>::value) {
		return maximum >= 0;
	}
	return
		minimum <= basic_numeric_limits<T>::max() and
		basic_numeric_limits<T>::min() <= maximum;
}

template<typename T>
constexpr auto type_fits_in_range(intmax_t const minimum, intmax_t const maximum) noexcept {
	static_assert(basic_numeric_limits<T>::is_specialized, "Only works with integer types.");
	if (std::is_same<T, uintmax_t>::value) {
		return false;
	}
	return
		minimum <= basic_numeric_limits<T>::min() and
		basic_numeric_limits<T>::max() <= maximum;
}

template<typename T1, typename T2, typename Enabler = void>
struct types_overlap {
	static constexpr bool value = false;
};

template<>
struct types_overlap<uintmax_t, uintmax_t, void> {
	static constexpr bool value = true;
};

template<typename T1, typename T2>
struct types_overlap<T1, T2, std::enable_if_t<basic_numeric_limits<T1>::is_specialized and basic_numeric_limits<T2>::is_specialized>> {
private:
	using type = std::decay_t<std::conditional_t<std::is_same<T1, uintmax_t>::value, T1, T2>>;
	using range_type = std::decay_t<std::conditional_t<std::is_same<T1, uintmax_t>::value, T2, T1>>;
public:
	static constexpr bool value = type_overlaps_range<type>(
		basic_numeric_limits<range_type>::min(),
		basic_numeric_limits<range_type>::max()
	);
};

}	// namespace detail
}	// namespace bounded
