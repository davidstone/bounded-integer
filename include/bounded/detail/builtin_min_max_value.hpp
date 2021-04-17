// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/is_bounded_integer.hpp>
#include <bounded/detail/min_max_value.hpp>
#include <bounded/normalize.hpp>

namespace bounded {

template<typename T> requires(!std::is_same_v<decltype(max_value<T>), detail::incomplete>)
inline constexpr auto builtin_max_value = normalize<max_value<T>>;

template<typename T> requires(!std::is_same_v<decltype(max_value<T>), detail::incomplete>)
inline constexpr auto builtin_min_value = normalize<min_value<T>>;

} // namespace bounded
