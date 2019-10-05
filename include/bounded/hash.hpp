// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>

#include <functional>

namespace std {

template<auto minimum, auto maximum, typename overflow_policy>
struct hash<bounded::integer<minimum, maximum, overflow_policy>> {
	using argument_type = bounded::integer<minimum, maximum, overflow_policy>;
	constexpr decltype(auto) operator()(argument_type const & argument) const {
		using underlying_type = typename argument_type::underlying_type;
		return hash<underlying_type>{}(static_cast<underlying_type>(argument));
	}
};

}	// namespace std