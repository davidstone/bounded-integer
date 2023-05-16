// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <bounded/assume.hpp>

export module bounded.assume;

import std_module;

static_assert(std::is_void_v<decltype(BOUNDED_ASSUME(true))>);

// Ensure BOUNDED_ASSUME is usable in a constant expression
static_assert((void([]() { BOUNDED_ASSUME(true); }()), true));
