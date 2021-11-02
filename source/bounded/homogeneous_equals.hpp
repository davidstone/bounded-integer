// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

template<typename T>
constexpr auto homogeneous_equals(T const & result, T const & expected) -> bool {
	return result == expected;
}
