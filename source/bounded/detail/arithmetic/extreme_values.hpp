// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/class.hpp>

#include <numeric_traits/int128.hpp>
#include <numeric_traits/min_max_value.hpp>

#pragma once

constexpr auto signed_max = bounded::constant<numeric_traits::max_value<numeric_traits::max_signed_t>>;
constexpr auto signed_min = bounded::constant<numeric_traits::min_value<numeric_traits::max_signed_t>>;
constexpr auto unsigned_max = bounded::constant<numeric_traits::max_value<numeric_traits::max_unsigned_t>>;
