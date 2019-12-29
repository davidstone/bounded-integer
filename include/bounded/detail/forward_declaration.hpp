// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/is_bounded_integer.hpp>

namespace bounded {

template<auto minimum, auto maximum, typename overflow_policy>
struct integer;

namespace detail {

template<auto minimum, auto maximum, typename overflow_policy>
inline constexpr auto is_bounded_integer<integer<minimum, maximum, overflow_policy>> = true;

} // namespace detail

// Does not verify that the value is in range with the policy
constexpr struct non_check_t{} non_check ;

}	// namespace bounded
