// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/assume_in_range.hpp>
#include <bounded/string.hpp>

#include <numeric_traits/min_max_value.hpp>

#include <stdexcept>

namespace bounded {

template<typename Exception = std::range_error>
constexpr auto check_in_range(integral auto const value, bounded_integer auto const minimum, bounded_integer auto const maximum) {
	if (minimum <= value and value <= maximum) {
		return ::bounded::detail::assume_in_range_impl(value, minimum, maximum);
	} else {
		throw Exception("Got a value of " + to_string(value) + " but expected a value in the range [" + to_string(minimum) + ", " + to_string(maximum) + "]");
	}
}

template<typename Target, typename Exception = std::range_error>
constexpr auto check_in_range(integral auto const value) {
	return ::bounded::check_in_range<Exception>(value, numeric_traits::min_value<Target>, numeric_traits::max_value<Target>);
}

} // namespace bounded
