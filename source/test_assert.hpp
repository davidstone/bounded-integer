// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOUNDED_TEST_ASSERT_HPP
#define BOUNDED_TEST_ASSERT_HPP

#if defined NDEBUG
	#define BOUNDED_NDEBUG_WAS_DEFINED NDEBUG
	#undef NDEBUG
#endif

#include <cassert>

// Like assert, but always evaluated
#define BOUNDED_TEST(...) assert((__VA_ARGS__))

#if defined BOUNDED_NDEBUG_WAS_DEFINED
	#define NDEBUG BOUNDED_NDEBUG_WAS_DEFINED
	#undef BOUNDED_NDEBUG_WAS_DEFINED
#endif

#endif // BOUNDED_TEST_ASSERT_HPP