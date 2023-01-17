// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <numeric_traits/has_int128.hpp>

export module bounded.underlying_type;

import bounded.comparison;
import bounded.overlapping_range;
import bounded.type;

import numeric_traits;

namespace bounded {
namespace detail {

template<typename T>
constexpr auto range_fits_in_type(auto const minimum, auto const maximum) {
	return value_fits_in_type<T>(minimum) and value_fits_in_type<T>(maximum);
}

template<auto...>
constexpr auto false_ = false;

template<auto minimum, auto maximum>
constexpr auto determine_type() {
	if constexpr (range_fits_in_type<unsigned char>(minimum, maximum)) {
		return type<unsigned char>;
	} else if constexpr (range_fits_in_type<signed char>(minimum, maximum)) {
		return type<signed char>;
	} else if constexpr (range_fits_in_type<unsigned short>(minimum, maximum)) {
		return type<unsigned short>;
	} else if constexpr (range_fits_in_type<signed short>(minimum, maximum)) {
		return type<signed short>;
	} else if constexpr (range_fits_in_type<unsigned int>(minimum, maximum)) {
		return type<unsigned int>;
	} else if constexpr (range_fits_in_type<signed int>(minimum, maximum)) {
		return type<signed int>;
	} else if constexpr (range_fits_in_type<unsigned long>(minimum, maximum)) {
		return type<unsigned long>;
	} else if constexpr (range_fits_in_type<signed long>(minimum, maximum)) {
		return type<signed long>;
	} else if constexpr (range_fits_in_type<unsigned long long>(minimum, maximum)) {
		return type<unsigned long long>;
	} else if constexpr (range_fits_in_type<signed long long>(minimum, maximum)) {
		return type<signed long long>;
#if defined NUMERIC_TRAITS_HAS_INT128
	} else if constexpr (range_fits_in_type<numeric_traits::uint128_t>(minimum, maximum)) {
		return type<numeric_traits::uint128_t>;
	} else if constexpr (range_fits_in_type<numeric_traits::int128_t>(minimum, maximum)) {
		return type<numeric_traits::int128_t>;
#endif
	} else {
		static_assert(false_<minimum, maximum>, "Bounds cannot fit in any type.");
	}
}

export template<auto minimum, auto maximum>
using underlying_type_t = typename decltype(determine_type<minimum, maximum>())::type;

} // namespace detail
} // namespace bounded
