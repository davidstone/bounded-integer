// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>

#include <climits>

namespace bounded {

template<typename T>
inline constexpr auto size_of = constant<sizeof(T)>;

namespace {

inline constexpr auto char_bit = constant<CHAR_BIT>;

}	// namespace

template<typename T>
inline constexpr auto size_of_bits = size_of<T> * char_bit;

}	// namespace bounded
