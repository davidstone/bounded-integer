// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/is_bounded_integer.hpp>

namespace bounded {

template<typename Integer> requires is_bounded_integer<Integer>
constexpr auto operator+(Integer const value) noexcept {
	return value;
}

}	// namespace bounded

