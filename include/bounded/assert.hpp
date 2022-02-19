// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/assume.hpp>

#include <cassert>
#include <type_traits>

#if defined NDEBUG or defined BOUNDED_NDEBUG
	#define BOUNDED_ASSERT(...) (std::is_constant_evaluated() ? BOUNDED_DETAIL_CONSTEXPR_ONLY_IF(__VA_ARGS__) : void())
	#define BOUNDED_ASSERT_OR_ASSUME BOUNDED_ASSUME
#else
	#define BOUNDED_ASSERT(...) (BOUNDED_DETAIL_PREVENT_COMMA(__VA_ARGS__), assert((__VA_ARGS__)))
	#define BOUNDED_ASSERT_OR_ASSUME BOUNDED_ASSERT
#endif
