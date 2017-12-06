// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/noexcept.hpp>

#include <utility>

namespace bounded {

// Other args allow you to declare the conversion as non_check
template<auto new_minimum, auto minimum, auto maximum, typename overflow_policy, storage_type storage, bool poisoned, typename... Args>
constexpr auto increase_min(integer<minimum, maximum, overflow_policy, storage, poisoned> const & value, Args && ... args) BOUNDED_NOEXCEPT_VALUE(
	integer<detail::safe_max(detail::normalize<new_minimum>, detail::normalize<minimum>), maximum, overflow_policy, storage, poisoned>(value, std::forward<Args>(args)...)
)

template<auto new_maximum, auto minimum, auto maximum, typename overflow_policy, storage_type storage, bool poisoned, typename... Args>
constexpr auto decrease_max(integer<minimum, maximum, overflow_policy, storage, poisoned> const & value, Args && ... args) BOUNDED_NOEXCEPT_VALUE(
	 integer<minimum, detail::safe_min(detail::normalize<new_maximum>, detail::normalize<maximum>), overflow_policy, storage, poisoned>(value, std::forward<Args>(args)...)
)

}	// namespace bounded
