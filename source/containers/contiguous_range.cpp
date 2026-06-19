// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.contiguous_range;

import containers.data;
import containers.sized_range;

import std_module;

namespace containers {

template<typename T>
concept is_pointer = std::is_pointer_v<T>;

template<typename Range>
concept has_data = requires(Range r) {
	{ containers::data(r) } -> is_pointer;
};

export template<typename Range>
concept contiguous_range = sized_range<Range> and has_data<Range>;

} // namespace containers
