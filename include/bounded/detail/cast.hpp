// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>
#include <bounded/detail/safe_extreme.hpp>

namespace bounded {

template<auto new_minimum, auto minimum, auto maximum>
constexpr auto increase_min(integer<minimum, maximum> const & value) {
	return ::bounded::assume_in_range(value, constant<detail::safe_max(new_minimum, minimum)>, constant<maximum>);
}
template<auto new_minimum, auto minimum, auto maximum>
constexpr auto increase_min(integer<minimum, maximum> const & value, unchecked_t) {
	return integer<normalize<detail::safe_max(new_minimum, minimum)>, maximum>(value, unchecked);
}

template<auto new_maximum, auto minimum, auto maximum>
constexpr auto decrease_max(integer<minimum, maximum> const & value) {
	return ::bounded::assume_in_range(value, constant<minimum>, constant<detail::safe_min(new_maximum, maximum)>);
}
template<auto new_maximum, auto minimum, auto maximum>
constexpr auto decrease_max(integer<minimum, maximum> const & value, unchecked_t) {
	return integer<minimum, normalize<detail::safe_min(new_maximum, maximum)>>(value, unchecked);
}

} // namespace bounded
