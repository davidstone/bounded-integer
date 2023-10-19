// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.data;

import containers.c_array;
import containers.has_member_size;

import std_module;

namespace containers {

template<typename Range>
concept has_member_data = requires(Range r) { r.data(); };

template<typename T>
concept class_contiguous_range = has_member_data<T> and has_member_size<T>;

export template<typename T>
concept contiguous_range = class_contiguous_range<T> or std::is_array_v<std::remove_reference_t<T>>;

export constexpr auto data(class_contiguous_range auto && r) {
	return r.data();
}

export template<typename T, std::size_t size>
constexpr auto data(c_array<T, size> const & a) -> T const * {
	return a;
}
export template<typename T, std::size_t size>
constexpr auto data(c_array<T, size> & a) -> T * {
	return a;
}
export template<typename T, std::size_t size>
constexpr auto data(c_array<T, size> && a) -> T * {
	return a;
}

} // namespace containers
