// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOUNDED_ASSERT_HPP
#define BOUNDED_ASSERT_HPP

#include <std_module/prelude.hpp>

#include <cassert>

import bounded.constexpr_only_if;
import bounded.prevent_comma;

import std_module;

// An expression of type `void`. This is used to allow using variadic
// function-like macros to support expressions that contain commas, such as
// `BOUNDED_ASSERT(f<a, b>())`, but prevents expressions that look like two
// arguments from being treated as a comma operator, such as
// `BOUNDED_ASSERT(false, "Error message")`.
#define BOUNDED_DETAIL_PREVENT_COMMA(...) \
	decltype(::bounded::detail::prevent_comma(__VA_ARGS__))()

#if defined NDEBUG or defined BOUNDED_NDEBUG
	#define BOUNDED_ASSERT(...) (std::is_constant_evaluated() ? ::bounded::detail::constexpr_only_if(__VA_ARGS__) : void())
	#define BOUNDED_ASSERT_OR_ASSUME(...) [[assume(__VA_ARGS__)]]
#else
	#define BOUNDED_ASSERT(...) (BOUNDED_DETAIL_PREVENT_COMMA(__VA_ARGS__), assert((__VA_ARGS__)))
	#define BOUNDED_ASSERT_OR_ASSUME BOUNDED_ASSERT
#endif

#endif // BOUNDED_ASSERT_HPP