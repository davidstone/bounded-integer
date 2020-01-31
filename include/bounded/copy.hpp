// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <operators/forward.hpp>
#include <bounded/concepts.hpp>

#include <type_traits>

namespace bounded {

// Works with explicit copy constructors
constexpr auto copy = []<typename T>(T && value) requires constructible_from<std::decay_t<T>, T &&> {
	return std::decay_t<T>(OPERATORS_FORWARD(value));
};

} // namespace bounded
