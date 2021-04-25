// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/normalize.hpp>

#include <numeric_traits/min_max_value.hpp>

namespace bounded {

template<numeric_traits::has_max_value T>
inline constexpr auto builtin_max_value = normalize<numeric_traits::max_value<T>>;

template<numeric_traits::has_min_value T>
inline constexpr auto builtin_min_value = normalize<numeric_traits::min_value<T>>;

} // namespace bounded
