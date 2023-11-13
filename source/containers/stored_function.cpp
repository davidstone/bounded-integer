// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.stored_function;

import bounded;
import std_module;

namespace containers {

// Use CTAD to avoid making a reference_wrapper<reference_wrapper>
template<typename Function>
using function_reference = decltype(std::reference_wrapper(bounded::declval<Function &>()));

// This used to check if the function is empty instead of whether it is small
// enough. The sizeof optimization is valid in all cases except those in which
// the address of state in the function is relevant. In those cases, the
// function should probably not be copyable (and definitely not trivially
// copyable), so it seems safe to do.
template<typename Function>
constexpr auto can_store_copy =
	std::invocable<Function const &> and
	bounded::mostly_trivial<Function> and
	sizeof(Function) <= sizeof(function_reference<Function>);

export template<typename Function>
using stored_function = std::conditional_t<
	can_store_copy<Function>,
	std::remove_const_t<Function>,
	function_reference<Function>
>;

} // namespace containers
