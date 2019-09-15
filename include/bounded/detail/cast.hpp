// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison.hpp>

namespace bounded {

// Other args allow you to declare the conversion as non_check
template<auto new_minimum, auto minimum, auto maximum, typename overflow_policy, typename... Args>
constexpr auto increase_min(integer<minimum, maximum, overflow_policy> const & value, Args... args) {
	return integer<detail::normalize<detail::safe_max(new_minimum, minimum)>, maximum, overflow_policy>(value, args...);
}

template<auto new_maximum, auto minimum, auto maximum, typename overflow_policy, typename... Args>
constexpr auto decrease_max(integer<minimum, maximum, overflow_policy> const & value, Args... args) {
	return integer<minimum, detail::normalize<detail::safe_min(new_maximum, maximum)>, overflow_policy>(value, args...);
}

template<typename Source, template<auto, auto, typename...> typename Target, typename... Extra>
using change_policy = Target<
	bounded::detail::normalize<Source::min().value()>,
	bounded::detail::normalize<Source::max().value()>,
	Extra...
>;

}	// namespace bounded
