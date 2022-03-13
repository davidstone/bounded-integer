// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/c_array.hpp>
#include <containers/erase_concepts.hpp>

#include <array>

namespace containers {

// `is_container` means that if you own a value of type `T`, you can safely move
// from any subset of the elements in the range.
template<typename T>
inline constexpr auto is_container =
	!std::is_lvalue_reference_v<T> and (
		detail::pop_backable<T> or
		detail::pop_frontable<T> or
		detail::erasable<T>
	);

template<typename T, std::size_t size>
inline constexpr auto is_container<containers::c_array<T, size>> = true;

template<typename T, std::size_t size>
inline constexpr auto is_container<std::array<T, size>> = true;

} // namespace containers
