// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/comparison.hpp>
#include <bounded/detail/int128.hpp>
#include <bounded/detail/overlapping_range.hpp>

namespace bounded {
namespace detail {

template<typename T, typename Minimum, typename Maximum>
constexpr auto range_fits_in_type(Minimum const minimum, Maximum const maximum) noexcept {
	return value_fits_in_type<T>(minimum) and value_fits_in_type<T>(maximum);
}

template<typename T>
struct type_c {
	using type = T;
};

template<auto...>
constexpr auto false_ = false;

template<auto minimum, auto maximum>
constexpr auto determine_type() noexcept {
	if constexpr (range_fits_in_type<unsigned char>(minimum, maximum)) {
		return type_c<unsigned char>{};
	} else if constexpr (range_fits_in_type<signed char>(minimum, maximum)) {
		return type_c<signed char>{};
	} else if constexpr (range_fits_in_type<unsigned short>(minimum, maximum)) {
		return type_c<unsigned short>{};
	} else if constexpr (range_fits_in_type<signed short>(minimum, maximum)) {
		return type_c<signed short>{};
	} else if constexpr (range_fits_in_type<unsigned int>(minimum, maximum)) {
		return type_c<unsigned int>{};
	} else if constexpr (range_fits_in_type<signed int>(minimum, maximum)) {
		return type_c<signed int>{};
	} else if constexpr (range_fits_in_type<unsigned long>(minimum, maximum)) {
		return type_c<unsigned long>{};
	} else if constexpr (range_fits_in_type<signed long>(minimum, maximum)) {
		return type_c<signed long>{};
	} else if constexpr (range_fits_in_type<unsigned long long>(minimum, maximum)) {
		return type_c<unsigned long long>{};
	} else if constexpr (range_fits_in_type<signed long long>(minimum, maximum)) {
		return type_c<signed long long>{};
#if defined BOUNDED_DETAIL_HAS_128_BIT
	} else if constexpr (range_fits_in_type<uint128_t>(minimum, maximum)) {
		return type_c<uint128_t>{};
	} else if constexpr (range_fits_in_type<int128_t>(minimum, maximum)) {
		return type_c<int128_t>{};
#endif
	} else {
		static_assert(false_<minimum, maximum>, "Bounds cannot fit in any type.");
	}
}

template<auto minimum, auto maximum>
using underlying_type_t = typename decltype(determine_type<minimum, maximum>())::type;

}	// namespace detail
}	// namespace bounded

