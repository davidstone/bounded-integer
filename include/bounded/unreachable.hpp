// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace bounded {

[[noreturn]] inline void unreachable() {
	// This needs the call to a builtin to avoid warnings
	#if defined __clang__ or defined __GNUC__ or defined __INTEL_COMPILER
		__builtin_unreachable();
	#elif defined _MSC_VER
		__assume(false);
	#endif
}

} // namespace bounded
