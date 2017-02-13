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


template<typename LHS, typename RHS>
constexpr auto types_overlap() {
	constexpr auto lhs_uintmax = std::is_same<LHS, std::uintmax_t>{};
	constexpr auto rhs_uintmax = std::is_same<RHS, std::uintmax_t>{};
	if constexpr (lhs_uintmax and rhs_uintmax) {
		return std::true_type{};
	} else if constexpr (basic_numeric_limits<LHS>::is_specialized and basic_numeric_limits<RHS>::is_specialized) {
		using type = std::decay_t<std::conditional_t<lhs_uintmax, LHS, RHS>>;
		using range_type = std::decay_t<std::conditional_t<lhs_uintmax, RHS, LHS>>;
		return type_overlaps_range<type>(
			basic_numeric_limits<range_type>::min(),
			basic_numeric_limits<range_type>::max()
		);
	} else {
		return std::false_type{};
	}
}

}	// namespace detail
}	// namespace bounded
