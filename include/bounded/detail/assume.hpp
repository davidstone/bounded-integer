// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

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

// Check is_constant_evaluated() first so that we do not evaluate the expression
// when assertions are disabled
#define BOUNDED_DETAIL_NON_CONSTEXPR_IF(...) ( \
	std::is_constant_evaluated() and !(__VA_ARGS__) ? \
		::bounded::detail::non_constexpr() : \
		void() \
)

#define BOUNDED_ASSUME(...) ( \
	static_cast<void>(BOUNDED_DETAIL_BUILTIN_ASSUME(__VA_ARGS__)), \
	BOUNDED_DETAIL_NON_CONSTEXPR_IF(__VA_ARGS__) \
)
