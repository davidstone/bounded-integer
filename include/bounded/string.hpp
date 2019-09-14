// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/is_bounded_integer.hpp>

#include <string>

namespace bounded {

// Import to_string for the numeric types
using std::to_string;

auto to_string(bounded_integer auto const x) {
	return to_string(+x.value());
}

using std::to_wstring;

auto to_wstring(bounded_integer auto const x) {
	return to_wstring(+x.value());
}

}	// namespace bounded

