// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.reference_wrapper;

import std_module;

namespace containers {

export template<typename T>
constexpr auto is_reference_wrapper = false;

template<typename T>
constexpr auto is_reference_wrapper<std::reference_wrapper<T>> = true;

// std::ref does not wrap T if it is already a reference_wrapper
export template<typename T>
constexpr auto make_reference_wrapper(T & ref) {
	return std::reference_wrapper<T>(ref);
}

export template<typename T>
constexpr auto && unwrap(T && reference) {
	if constexpr (is_reference_wrapper<std::remove_cvref_t<T>>) {
		return reference.get();
	} else {
		return OPERATORS_FORWARD(reference);
	}
}

} // namespace containers
