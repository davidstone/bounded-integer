// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.stored_function;

import bounded;
import std_module;

namespace containers {

export template<typename Function>
using stored_function = std::conditional_t<
	std::is_empty_v<Function> and std::is_trivially_copyable_v<Function>,
	std::remove_const_t<Function>,
	decltype(std::reference_wrapper(bounded::declval<Function &>()))
>;

} // namespace containers
