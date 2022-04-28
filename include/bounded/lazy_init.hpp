// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/concepts.hpp>

namespace bounded {

struct lazy_init_t {
} inline constexpr lazy_init;

template<typename Function, typename T>
concept construct_function_for = convertible_to<decltype(declval<Function>()()), T>;

} // namespace bounded