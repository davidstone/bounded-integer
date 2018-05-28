// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>

#include <functional>

namespace std {

template<auto minimum, auto maximum, typename overflow_policy, bool poisoned>
struct hash<bounded::integer<minimum, maximum, overflow_policy, poisoned>> {
	using argument_type = bounded::integer<minimum, maximum, overflow_policy, poisoned>;
	constexpr decltype(auto) operator()(argument_type const & argument) const {
		return hash<typename argument_type::underlying_type>{}(argument.value());
	}
};

}	// namespace std
