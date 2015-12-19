// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/forward_declaration.hpp>
#include <bounded/detail/noexcept.hpp>

#include <utility>

namespace bounded {

// Other args allow you to declare the conversion as non_check
template<intmax_t new_minimum, intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, bool poisoned, typename... Args>
constexpr auto increase_min(integer<minimum, maximum, overflow_policy, storage, poisoned> const & value, Args && ... args) BOUNDED_NOEXCEPT_VALUE(
	integer<(new_minimum > minimum) ? new_minimum : minimum, maximum, overflow_policy, storage, poisoned>(value, std::forward<Args>(args)...)
)

template<intmax_t new_maximum, intmax_t minimum, intmax_t maximum, typename overflow_policy, storage_type storage, bool poisoned, typename... Args>
constexpr auto decrease_max(integer<minimum, maximum, overflow_policy, storage, poisoned> const & value, Args && ... args) BOUNDED_NOEXCEPT_VALUE(
	 integer<minimum, (new_maximum < maximum) ? new_maximum : maximum, overflow_policy, storage, poisoned>(value, std::forward<Args>(args)...)
)

}	// namespace bounded
