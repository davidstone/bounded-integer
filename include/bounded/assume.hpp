// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

namespace bounded::detail {

inline void non_constexpr() {
}

void prevent_comma(auto &&);

} // namespace bounded::detail

// An expression of type `void`. This is used to allow using variadic
// function-like macros to support expressions that contain commas, such as
// `BOUNDED_ASSERT(f<a, b>())`, but prevents expressions that look like two
// arguments from being treated as a comma operator, such as
// `BOUNDED_ASSERT(false, "Error message")`.
#define BOUNDED_DETAIL_PREVENT_COMMA(...) \
	decltype(::bounded::detail::prevent_comma(__VA_ARGS__))()

#define BOUNDED_DETAIL_CONSTEXPR_ONLY_IF(...) ( \
	(__VA_ARGS__) ? \
		BOUNDED_DETAIL_PREVENT_COMMA(__VA_ARGS__) : \
		::bounded::detail::non_constexpr() \
)

#if defined __clang__ or defined __INTEL_COMPILER
	// https://github.com/llvm/llvm-project/issues/40658
	#define BOUNDED_ASSUME(...) ( \
		std::is_constant_evaluated() ? \
			BOUNDED_DETAIL_CONSTEXPR_ONLY_IF(__VA_ARGS__) : \
			__builtin_assume(__VA_ARGS__) \
	)
#elif defined _MSC_VER
	#define BOUNDED_ASSUME(...) ( \
		std::is_constant_evaluated() ? \
			BOUNDED_DETAIL_CONSTEXPR_ONLY_IF(__VA_ARGS__) : \
			__assume(__VA_ARGS__) \
	)
#elif defined __GNUC__
	#define BOUNDED_ASSUME(...) \
		(!(__VA_ARGS__) ? __builtin_unreachable() : BOUNDED_DETAIL_PREVENT_COMMA(__VA_ARGS__))
#else
	#define BOUNDED_ASSUME BOUNDED_DETAIL_CONSTEXPR_ONLY_IF
#endif
