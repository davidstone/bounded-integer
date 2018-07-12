// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/forward_declaration.hpp>
#include <type_traits>

namespace bounded {

template<typename T>
constexpr auto is_bounded_integer = false;

template<auto minimum, auto maximum, typename overflow_policy, bool poisoned>
constexpr auto is_bounded_integer<integer<minimum, maximum, overflow_policy, poisoned>> = true;

}	// namespace bounded
