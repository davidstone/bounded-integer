// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/common.hpp>
#include <bounded/detail/class.hpp>
#include <bounded/detail/is_bounded_integer.hpp>

namespace bounded {

template<typename T, bounded_integer Integer> requires(
	(std::is_pointer_v<T> or (std::is_array_v<T> and Integer::max() <= constant<std::extent_v<T>>))
)
constexpr auto operator+(T const & array, Integer const number) noexcept {
	return array + number.value();
}


template<bounded_integer Integer, typename T> requires(
	(std::is_pointer_v<T> or (std::is_array_v<T> and Integer::max() <= constant<std::extent_v<T>>))
)
constexpr auto operator+(Integer const number, T const & array) noexcept {
	return number.value() + array;
}



// Not possible to overload operator[]. See
// https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/CmBERU_sr8Y

}	// namespace bounded

