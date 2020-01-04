// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/value_to_function.hpp>
#include <operators/forward.hpp>
#include <operators/returns.hpp>

namespace bounded {

constexpr auto insert(auto & destination, auto position, auto && value) OPERATORS_RETURNS(
	destination.emplace(std::move(position), value_to_function(OPERATORS_FORWARD(value)))
)

constexpr auto insert(auto & destination, auto && value) OPERATORS_RETURNS(
	destination.emplace(value_to_function(OPERATORS_FORWARD(value)))
)

} // namespace bounded