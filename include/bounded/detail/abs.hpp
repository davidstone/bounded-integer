// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/cast.hpp>
#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/minmax.hpp>
#include <bounded/detail/arithmetic/unary_minus.hpp>

namespace bounded {

template<bounded_integer Integer>
constexpr auto abs(Integer const value) noexcept {
	if constexpr (Integer::min() >= constant<0>) {
		return value;
	} else if constexpr (Integer::max() <= constant<0>) {
		return -value;
	} else {
		// The 0 has to be there to restrict the range of possible values. Without
		// it, abs(integer<-7, 3>) would be [-3, 7] instead of [0, 7].
		return increase_min<0>(max(value, -value), non_check);
	}
}

}	// namespace bounded
