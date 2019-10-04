// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/comparison.hpp>
#include <bounded/detail/min_max_value.hpp>

namespace bounded {
namespace detail {

template<typename T>
constexpr auto as_builtin_integer(T const value) {
	if constexpr (is_bounded_integer<T>) {
		return value.value();
	} else {
		return value;
	}
}

template<typename T> requires(!std::is_same_v<decltype(max_value<T>), incomplete>)
inline constexpr auto builtin_max_value = as_builtin_integer(max_value<T>);

template<typename T> requires(!std::is_same_v<decltype(max_value<T>), incomplete>)
inline constexpr auto builtin_min_value = as_builtin_integer(min_value<T>);

template<auto minimum, auto maximum, typename policy>
inline constexpr auto builtin_max_value<integer<minimum, maximum, policy>> = maximum;

template<auto minimum, auto maximum, typename policy>
inline constexpr auto builtin_min_value<integer<minimum, maximum, policy>> = minimum;

template<typename T, typename Value>
constexpr auto value_fits_in_type(Value const value) {
	return compare(builtin_min_value<T>, value) <= 0 and compare(value, builtin_max_value<T>) <= 0;
}

}	// namespace detail
}	// namespace bounded
