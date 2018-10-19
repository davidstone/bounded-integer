// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/requires.hpp>

#include <string>
#include <type_traits>

namespace bounded {

// Import to_string for the numeric types
using std::to_string;

template<typename Integer, BOUNDED_REQUIRES(is_bounded_integer<Integer>)>
auto to_string(Integer const & x) {
	return to_string(+x.value());
}

using std::to_wstring;

template<typename Integer, BOUNDED_REQUIRES(is_bounded_integer<Integer>)>
auto to_wstring(Integer const & x) {
	return to_wstring(+x.value());
}

}	// namespace bounded

