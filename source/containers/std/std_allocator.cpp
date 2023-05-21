// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.std.std_allocator;

import std_module;

namespace std_containers {

template<typename T>
constexpr auto is_std_allocator = false;

template<typename T>
constexpr auto is_std_allocator<std::allocator<T>> = true;

export template<typename T>
concept std_allocator = is_std_allocator<T>;

} // namespace std_containers
