// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/forward_declaration.hpp>
#include <bounded/detail/policy/all.hpp>

namespace bounded {

template<auto minimum, auto maximum, typename Exception = policy_detail::default_exception>
using checked_integer = integer<minimum, maximum, throw_policy<Exception>>;

template<auto minimum, auto maximum>
using clamped_integer = integer<minimum, maximum, clamp_policy>;

template<auto minimum, auto maximum>
using wrapping_integer = integer<minimum, maximum, modulo_policy>;

}	// namespace bounded

