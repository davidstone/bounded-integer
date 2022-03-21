// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <utility>

namespace bounded {

#if defined __cpp_lib_unreachable
using std::unreachable;
#else
[[noreturn]] inline void unreachable() {
	// This needs the call to a builtin to avoid warnings
	__builtin_unreachable();
}
#endif

} // namespace bounded
