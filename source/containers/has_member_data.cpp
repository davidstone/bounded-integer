// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.has_member_data;

import std_module;

namespace containers {

template<typename T>
concept pointer = std::is_pointer_v<T>;

export template<typename T>
concept has_member_data = requires(T r) {
	{ r.data() } -> pointer;
};

} // namespace containers
