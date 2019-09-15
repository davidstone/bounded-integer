// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

// Mostly the same as returning decltype(auto), but with expression SFINAE
#define BOUNDED_RETURNS(...) -> decltype(__VA_ARGS__) { return __VA_ARGS__; }
