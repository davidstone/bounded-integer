// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/assert.hpp>


#if defined __clang__ or defined __INTEL_COMPILER
    #define BOUNDED_DETAIL_BUILTIN_UNREACHABLE __builtin_unreachable
#elif defined _MSC_VER
	#define BOUNDED_DETAIL_BUILTIN_UNREACHABLE() __assume(false)
#elif defined __GNUC__
    #define BOUNDED_DETAIL_BUILTIN_UNREACHABLE __builtin_unreachable
#endif

namespace bounded {

[[noreturn]] inline void unreachable() {
	// This needs the call to a builtin to avoid warnings
	BOUNDED_DETAIL_BUILTIN_UNREACHABLE();
}

[[noreturn]] inline void assert_or_assume_unreachable() {
	BOUNDED_ASSERT_OR_ASSUME(false);
}

} // namespace bounded