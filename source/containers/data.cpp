// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.data;

import containers.c_array;
import containers.has_member_data;
import containers.has_member_size;
import containers.range;

import bounded;
import std_module;

namespace containers {

export template<range Range> requires has_member_data<Range>
constexpr auto data(Range && r) {
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
