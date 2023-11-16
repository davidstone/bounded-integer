// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.stored_function;

import containers.reference_or_value;

import bounded;
import std_module;

namespace containers {

export template<typename Function>
using stored_function = std::conditional_t<
	std::invocable<Function const &>,
	reference_or_value<Function>,
	std::reference_wrapper<Function>
>;

} // namespace containers
