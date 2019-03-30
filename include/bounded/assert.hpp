// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/assume.hpp>

#include <cassert>
#include <cstdlib>

#if defined NDEBUG or defined BOUNDED_NDEBUG
	#define BOUNDED_ASSERT(...) void(sizeof(__VA_ARGS__))
	#define BOUNDED_ASSERT_OR_ASSUME BOUNDED_ASSUME
#else
	// This is not defined to just be assert to support expressions with commas,
	// but is still carefully written to not defeat warnings for expressions
	// like `assert(false, "My error message")` which actually would invoke the
	// comma operator.
	#define BOUNDED_ASSERT(...) (static_cast<bool>(__VA_ARGS__) ? void(0) : assert((__VA_ARGS__)))
	#define BOUNDED_ASSERT_OR_ASSUME BOUNDED_ASSERT
#endif
