// Copyright David Stone 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/detail/class.hpp>
#include <bounded/detail/max_builtin.hpp>
#include <limits>

constexpr auto signed_max = bounded::constant<bounded::basic_numeric_limits<bounded::detail::max_signed_t>::max()>;
constexpr auto signed_min = bounded::constant<bounded::basic_numeric_limits<bounded::detail::max_signed_t>::min()>;
constexpr auto unsigned_max = bounded::constant<bounded::basic_numeric_limits<bounded::detail::max_unsigned_t>::max()>;
