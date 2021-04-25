// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/cast.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/minmax.hpp>
#include <bounded/detail/arithmetic/unary_minus.hpp>

#include <numeric_traits/min_max_value.hpp>

namespace bounded {

constexpr auto abs(bounded_integer auto const value) {
	using Integer = decltype(value);
	if constexpr (numeric_traits::min_value<Integer> >= constant<0>) {
		return value;
	} else if constexpr (numeric_traits::max_value<Integer> <= constant<0>) {
		return -value;
	} else {
		// The 0 has to be there to restrict the range of possible values. Without
		// it, abs(integer<-7, 3>) would be [-3, 7] instead of [0, 7].
		return increase_min<0>(max(value, -value), non_check);
	}
}

}	// namespace bounded
