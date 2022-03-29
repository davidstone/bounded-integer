// Copyright David Stone 2022.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/class.hpp>
#include <bounded/detail/is_bounded_integer.hpp>

#include <numeric_traits/min_max_value.hpp>

namespace bounded {

template<isomorphic_to_integral T>
inline constexpr auto number_of = constant<numeric_traits::max_value<T>> - constant<numeric_traits::min_value<T>> + constant<1>;

} // namespace bounded
