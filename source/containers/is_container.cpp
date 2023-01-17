// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.is_container;

import containers.c_array;
import containers.erase_concepts;
import containers.pop_back;
import containers.range_reference_t;

import std_module;

namespace containers {

template<typename T>
concept range_reference_t_is_reference = std::is_reference_v<range_reference_t<T>>;

// `is_container` means that if you own a value of type `T`, you can safely move
// from any subset of the elements in the range.
export template<typename T>
constexpr auto is_container =
	!std::is_lvalue_reference_v<T> and
	range_reference_t_is_reference<T> and (
		pop_backable<T> or
		pop_frontable<T> or
		erasable<T>
	);

template<typename T, std::size_t size>
constexpr auto is_container<containers::c_array<T, size>> = true;

template<typename T, std::size_t size>
constexpr auto is_container<std::array<T, size>> = true;

} // namespace containers

static_assert(!containers::is_container<void>);
static_assert(!containers::is_container<int>);
static_assert(!containers::is_container<std::span<int>>);
static_assert(!containers::is_container<std::string_view>);