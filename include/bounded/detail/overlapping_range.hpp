// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/basic_numeric_limits.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/forward_declaration.hpp>

#include <cstdint>
#include <type_traits>

// All of these functions have a precondition that minimum <= maximum

namespace bounded {
namespace detail {

template<typename T>
constexpr auto value_fits_in_type(intmax_t const value) noexcept {
	static_assert(basic_numeric_limits<T>::is_specialized, "Only works with integer types.");
	if constexpr (std::is_same<T, uintmax_t>{}) {
		return value >= 0;
	} else {
		using limits = basic_numeric_limits<T>;
		return limits::min() <= value and value <= limits::max();
	}
}

template<typename T>
constexpr auto type_overlaps_range(intmax_t const minimum, intmax_t const maximum) noexcept {
	static_assert(basic_numeric_limits<T>::is_specialized, "Only works with integer types.");
	if constexpr (std::is_same<T, uintmax_t>{}) {
		static_cast<void>(minimum);
		return maximum >= 0;
	} else {
		using limits = basic_numeric_limits<T>;
		return minimum <= limits::max() and limits::min() <= maximum;
	}
}

template<typename T>
constexpr auto type_fits_in_range(intmax_t const minimum, intmax_t const maximum) noexcept {
	static_assert(basic_numeric_limits<T>::is_specialized, "Only works with integer types.");
	if constexpr (std::is_same<T, uintmax_t>{}) {
		return std::false_type{};
	} else {
		using limits = basic_numeric_limits<T>;
		return minimum <= limits::min() and limits::max() <= maximum;
	}
}

}	// namespace detail
}	// namespace bounded
