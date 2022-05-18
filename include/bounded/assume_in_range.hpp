// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/builtin_min_max_value.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/safe_extreme.hpp>
#include <bounded/normalize.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <utility>

namespace bounded {

struct unchecked_t {
};
inline constexpr auto unchecked = unchecked_t();

namespace detail {

template<integral T, bounded_integer Minimum, bounded_integer Maximum>
constexpr auto assume_in_range_impl(T const value, Minimum, Maximum) {
	return integer<
		normalize<detail::safe_max(builtin_min_value<Minimum>, builtin_min_value<T>)>,
		normalize<detail::safe_min(builtin_max_value<Maximum>, builtin_max_value<T>)>
	>(value, unchecked);
}

} // namespace detail

constexpr auto assume_in_range(integral auto const value, bounded_integer auto const minimum, bounded_integer auto const maximum) {
	if (value < minimum or maximum < value) {
		std::unreachable();
	}
	return ::bounded::detail::assume_in_range_impl(value, minimum, maximum);
}

template<integral Target>
constexpr auto assume_in_range(integral auto const value) {
	constexpr auto minimum = numeric_traits::min_value<Target>;
	constexpr auto maximum = numeric_traits::max_value<Target>;
	if (integer(value) < integer(minimum) or integer(maximum) < integer(value)) {
		std::unreachable();
	}
	if constexpr (bounded_integer<Target>) {
		return ::bounded::detail::assume_in_range_impl(value, minimum, maximum);
	} else {
		return static_cast<Target>(value);
	}
}

} // namespace bounded
