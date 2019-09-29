// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#if defined __clang__
	#define BOUNDED_ASSUME __builtin_assume
#elif defined _MSC_VER
	#define BOUNDED_ASSUME __assume
#elif defined __GNUC__
	#define BOUNDED_ASSUME(...) \
        do { \
            if (!(__VA_ARGS__)) { \
                __builtin_unreachable(); \
            } \
        } while (false)
#else
	#define BOUNDED_ASSUME(...) void(sizeof(__VA_ARGS__))
#endif
