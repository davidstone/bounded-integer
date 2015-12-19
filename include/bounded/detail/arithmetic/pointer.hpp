// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/arithmetic/common.hpp>

#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/requires.hpp>

namespace bounded {

template<typename T, typename Integer, BOUNDED_REQUIRES(is_bounded_integer<Integer>)>
constexpr auto operator+(T * const pointer, Integer const & number) noexcept {
	return pointer + number.value();
}

template<typename Integer, typename T, BOUNDED_REQUIRES(is_bounded_integer<Integer>)>
constexpr auto operator+(Integer const & number, T * const pointer) noexcept {
	return number.value() + pointer;
}

// Not possible to overload operator[]. See
// https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/CmBERU_sr8Y

}	// namespace bounded

