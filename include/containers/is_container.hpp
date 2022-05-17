// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/c_array.hpp>
#include <containers/erase_concepts.hpp>
#include <containers/range_reference_t.hpp>

#include <array>

namespace containers {
namespace detail {

template<typename T>
concept range_reference_t_is_reference = std::is_reference_v<range_reference_t<T>>;

} // namespace detail

// `is_container` means that if you own a value of type `T`, you can safely move
// from any subset of the elements in the range.
template<typename T>
inline constexpr auto is_container =
	!std::is_lvalue_reference_v<T> and
	detail::range_reference_t_is_reference<T> and (
		detail::pop_backable<T> or
		detail::pop_frontable<T> or
		detail::erasable<T>
	);

template<typename T, std::size_t size>
inline constexpr auto is_container<containers::c_array<T, size>> = true;

template<typename T, std::size_t size>
inline constexpr auto is_container<std::array<T, size>> = true;

} // namespace containers
