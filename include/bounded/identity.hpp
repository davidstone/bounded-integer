// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <operators/forward.hpp>

#include <type_traits>

namespace bounded {

inline constexpr auto identity = [](auto && value) -> auto && {
	return OPERATORS_FORWARD(value);
};
using identity_t = std::decay_t<decltype(identity)>;

} // namespace bounded