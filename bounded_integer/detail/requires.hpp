// Improved version of enable_if
// Copyright (C) 2015 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

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

