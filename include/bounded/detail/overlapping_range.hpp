// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/basic_numeric_limits.hpp>
#include <bounded/detail/comparison.hpp>

// All of these functions have a precondition that minimum <= maximum

namespace bounded {
namespace detail {

template<typename T, typename Value>
constexpr auto value_fits_in_type(Value const value) {
	static_assert(basic_numeric_limits<T>::is_specialized, "Only works with integer types.");
	using limits = basic_numeric_limits<T>;
	return compare(limits::min(), value) <= 0 and compare(value, limits::max()) <= 0;
}

}	// namespace detail
}	// namespace bounded
