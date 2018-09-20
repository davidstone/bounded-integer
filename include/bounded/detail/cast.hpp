// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/noexcept.hpp>

namespace bounded {

// Other args allow you to declare the conversion as non_check
template<auto new_minimum, auto minimum, auto maximum, typename overflow_policy, typename... Args>
constexpr auto increase_min(integer<minimum, maximum, overflow_policy> const & value, Args... args) BOUNDED_NOEXCEPT_VALUE(
	integer<detail::normalize<detail::safe_max(new_minimum, minimum)>, maximum, overflow_policy>(value, args...)
)

template<auto new_maximum, auto minimum, auto maximum, typename overflow_policy, typename... Args>
constexpr auto decrease_max(integer<minimum, maximum, overflow_policy> const & value, Args... args) BOUNDED_NOEXCEPT_VALUE(
	 integer<minimum, detail::normalize<detail::safe_min(new_maximum, maximum)>, overflow_policy>(value, args...)
)

}	// namespace bounded
