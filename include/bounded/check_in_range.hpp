// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/builtin_min_max_value.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/safe_extreme.hpp>
#include <bounded/normalize.hpp>
#include <bounded/string.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <stdexcept>

namespace bounded {
namespace detail {

template<typename T, auto minimum, auto maximum>
constexpr auto reduce_range(T const value, constant_t<minimum>, constant_t<maximum>) {
	return integer<
		normalize<detail::safe_max(minimum, builtin_min_value<T>)>,
		normalize<detail::safe_min(maximum, builtin_max_value<T>)>
	>(value, non_check);
}

} // namespace detail

template<typename Exception = std::range_error, bounded_integer Minimum, bounded_integer Maximum>
constexpr auto check_in_range(bounded_integer auto const value, Minimum const minimum, Maximum const maximum) {
	if (minimum <= value and value <= maximum) {
		return detail::reduce_range(value, numeric_traits::min_value<Minimum>, numeric_traits::max_value<Maximum>);
	} else {
		throw Exception("Got a value of " + to_string(value) + " but expected a value in the range [" + to_string(minimum) + ", " + to_string(maximum) + "]");
	}
}

template<typename Target, typename Exception = std::range_error>
constexpr auto check_in_range(bounded_integer auto const value) {
	return check_in_range<Exception>(value, numeric_traits::min_value<Target>, numeric_traits::max_value<Target>);
}

} // namespace bounded
