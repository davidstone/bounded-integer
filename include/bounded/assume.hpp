// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOUNDED_ASSUME_HPP
#define BOUNDED_ASSUME_HPP

import bounded.constexpr_only_if;

import std_module;

#if defined __clang__ or defined __INTEL_COMPILER
	// https://github.com/llvm/llvm-project/issues/40658
	#define BOUNDED_ASSUME(...) ( \
		std::is_constant_evaluated() ? \
			::bounded::detail::constexpr_only_if(__VA_ARGS__) : \
			__builtin_assume(__VA_ARGS__) \
	)
#elif defined _MSC_VER
	#define BOUNDED_ASSUME(...) ( \
		std::is_constant_evaluated() ? \
			::bounded::detail::constexpr_only_if(__VA_ARGS__) : \
			__assume(__VA_ARGS__) \
	)
#else
	#define BOUNDED_ASSUME(...) \
		[[assume(__VA_ARGS__)]]
#endif

#endif // BOUNDED_ASSUME_HPP