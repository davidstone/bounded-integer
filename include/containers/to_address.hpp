// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

namespace containers {
namespace detail {

template<typename T>
concept pointer_like = std::is_pointer_v<T> or std::is_null_pointer_v<T>;

template<typename T>
concept to_addressable = pointer_like<T> or requires(T const x) { x.to_address(); };

} // namespace detail

template<detail::to_addressable Iterator>
constexpr auto to_address(Iterator const it) {
	if constexpr (detail::pointer_like<Iterator>) {
		return it;
	} else {
		return it.to_address();
	}
}

} // namespace containers
