// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

// This must use a variadic macro in case the argument has a comma
#define BOUNDED_FORWARD(...) static_cast<decltype(__VA_ARGS__) &&>(__VA_ARGS__)

