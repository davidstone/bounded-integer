// Copyright David Stone 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

namespace containers {

template<typename T>
using uninitialized_storage = std::aligned_storage_t<sizeof(T), alignof(T)>;

}	// namespace containers
