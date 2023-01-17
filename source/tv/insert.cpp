// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>
#include <operators/returns.hpp>

export module tv.insert;

import bounded;
import std_module;

namespace tv {

export constexpr auto insert(auto & destination, auto position, auto && value) OPERATORS_RETURNS(
	destination.emplace(std::move(position), bounded::value_to_function(OPERATORS_FORWARD(value)))
)

export constexpr auto insert(auto & destination, auto && value) OPERATORS_RETURNS(
	destination.emplace(bounded::value_to_function(OPERATORS_FORWARD(value)))
)

} // namespace tv