// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.cheaply_sortable;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

// Size determined experimentally, but also the size of a cache line. Maybe it
// should be `std::hardware_destructive_interference_size`?
export template<typename T>
concept cheaply_sortable = std::is_trivially_copyable_v<T> and sizeof(T) <= 64_bi;

} // namespace containers