// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstddef>

namespace containers {

template<typename T, std::size_t size>
using c_array = T[size];

// Using this as a parameter allows construction from `{}` to work
struct empty_c_array_parameter {
};

} // namespace containers
