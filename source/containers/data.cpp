// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

module;

#include <operators/forward.hpp>

export module containers.data;

import containers.c_array;
import containers.has_member_begin;
import containers.has_member_data;
import containers.has_member_size;
import containers.range;
import containers.to_address;

import bounded;
import std_module;

namespace containers {

template<typename Range>
concept addressable_begin = requires(Range r) {
	{ OPERATORS_FORWARD(r).begin() } -> to_addressable;
};

export template<range Range>
	requires has_member_data<Range> or addressable_begin<Range>
constexpr auto data(Range && r) {
	if constexpr (has_member_data<Range>) {
		return r.data();
	} else {
		return containers::to_address(OPERATORS_FORWARD(r.begin()));
	}
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
