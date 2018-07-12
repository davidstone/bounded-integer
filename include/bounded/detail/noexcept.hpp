// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

// These must use a variadic macro in case the argument has a comma

#define BOUNDED_NOEXCEPT_INITIALIZATION(...) noexcept(noexcept(__VA_ARGS__)): __VA_ARGS__

// No SFINAE
#define BOUNDED_NOEXCEPT(...) noexcept(noexcept(__VA_ARGS__)) { return __VA_ARGS__; }

#if defined __GNUC__ && !defined __clang__
	#define BOUNDED_NOEXCEPT_GCC_BUG(...) { return __VA_ARGS__; }
#else
	#define BOUNDED_NOEXCEPT_GCC_BUG(...) noexcept(noexcept(__VA_ARGS__)) { return __VA_ARGS__; }
#endif

// No SFINAE
#define BOUNDED_NOEXCEPT_VOID(...) noexcept(noexcept(__VA_ARGS__)) { __VA_ARGS__; }

// Same as returning decltype(auto), but with expression SFINAE
#define BOUNDED_NOEXCEPT_DECLTYPE(...) \
	noexcept( \
		noexcept(__VA_ARGS__) and \
		std::is_nothrow_move_constructible_v<decltype(__VA_ARGS__)> \
	) -> decltype(__VA_ARGS__) { \
		return __VA_ARGS__; \
	}

// Same as returning auto, but with expression SFINAE
#define BOUNDED_NOEXCEPT_VALUE(...) \
	noexcept( \
		noexcept(__VA_ARGS__) and \
		std::is_nothrow_constructible<std::decay_t<decltype(__VA_ARGS__)>, decltype(__VA_ARGS__)>{} \
	) -> std::decay_t<decltype(__VA_ARGS__)> { \
		return __VA_ARGS__; \
	}

// Same as returning auto &, but with expression SFINAE
#define BOUNDED_NOEXCEPT_REF(...) \
	noexcept( \
		noexcept(__VA_ARGS__) \
	) -> decltype(__VA_ARGS__) & { \
		return __VA_ARGS__; \
	}

// Same as returning auto const &, but with expression SFINAE
#define BOUNDED_NOEXCEPT_CONST_REF(...) \
	noexcept( \
		noexcept(__VA_ARGS__) \
	) -> decltype(__VA_ARGS__) const & { \
		return __VA_ARGS__; \
	}

// Same as returning auto &&, but with expression SFINAE
#define BOUNDED_NOEXCEPT_REF_REF(...) \
	noexcept( \
		noexcept(__VA_ARGS__) \
	) -> decltype(__VA_ARGS__) && { \
		return __VA_ARGS__; \
	}
