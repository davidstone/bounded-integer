// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.reference_or_value;

import containers.value_wrapper;

import bounded;
import std_module;

namespace containers {

template<typename T>
constexpr auto can_store_copy =
	bounded::mostly_trivial<T> and
	sizeof(T) <= sizeof(std::reference_wrapper<T>);

export template<typename T>
using reference_or_value = std::conditional_t<
	can_store_copy<T>,
	value_wrapper<std::remove_const_t<T>>,
	std::reference_wrapper<T const>
>;

} // namespace containers
