// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#if defined __clang__ or defined __INTEL_COMPILER
	#define BOUNDED_DETAIL_BUILTIN_ASSUME __builtin_assume
#elif defined _MSC_VER
	#define BOUNDED_DETAIL_BUILTIN_ASSUME __assume
#elif defined __GNUC__
	#define BOUNDED_DETAIL_BUILTIN_ASSUME(...) \
        (!(__VA_ARGS__) ? __builtin_unreachable() : void())
#else
    #define BOUNDED_DETAIL_BUILTIN_ASSUME(...) void(sizeof(__VA_ARGS__))
#endif

namespace bounded::detail {

inline void non_constexpr() {
}

} // namespace bounded::detail

// This is guaranteed to not be a constant expression if the condition is false.
#define BOUNDED_ASSUME(...) ( \
    BOUNDED_DETAIL_BUILTIN_ASSUME(__VA_ARGS__), \
    !(__VA_ARGS__) ? \
        ::bounded::detail::non_constexpr() : \
        void() \
)
