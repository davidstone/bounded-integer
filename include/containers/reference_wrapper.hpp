// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <operators/forward.hpp>

#include <functional>
#include <type_traits>

namespace containers::detail {

template<typename T>
inline constexpr auto is_reference_wrapper = false;

template<typename T>
inline constexpr auto is_reference_wrapper<std::reference_wrapper<T>> = true;

template<typename T>
constexpr auto && unwrap(T && reference) {
	if constexpr (is_reference_wrapper<std::remove_cvref_t<T>>) {
		return reference.get();
	} else {
		return OPERATORS_FORWARD(reference);
	}
}

} // namespace containers::detail
