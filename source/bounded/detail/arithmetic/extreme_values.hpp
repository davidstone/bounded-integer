// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/class.hpp>
#include <bounded/detail/max_builtin.hpp>

#pragma once

constexpr auto signed_max = bounded::constant<bounded::max_value<bounded::detail::max_signed_t>>;
constexpr auto signed_min = bounded::constant<bounded::min_value<bounded::detail::max_signed_t>>;
constexpr auto unsigned_max = bounded::constant<bounded::max_value<bounded::detail::max_unsigned_t>>;
