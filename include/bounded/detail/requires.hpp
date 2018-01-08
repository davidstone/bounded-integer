// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

// http://flamingdangerzone.com/cxx11/2012/06/01/almost-static-if.html
// gave an outline of most of the tricks in here. This uses a macro to simplify
// usage and give better error messages.

#include <type_traits>

#if 0

// Usage:
template<typename Blah, BOUNDED_REQUIRES(condition<Blah>)>
void f() {
}

#endif

namespace bounded {
namespace detail {
enum class enabler {};
}	// namespace detail
}	// namespace bounded

// This must use a variadic macro in case the argument has a comma
#define BOUNDED_REQUIRES(...) \
	typename std::enable_if<__VA_ARGS__, bounded::detail::enabler>::type = bounded::detail::enabler{}

