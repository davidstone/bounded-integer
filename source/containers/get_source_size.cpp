// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.get_source_size;

import containers.range_size_t;
import containers.size;
import containers.size_then_use_range;

import bounded;

namespace containers {

export template<typename Target>
constexpr auto get_source_size(size_then_use_range auto && source) {
	auto const value = containers::linear_size(source);
	if constexpr (bounded::bounded_integer<decltype(value)>) {
		return value;
	} else {
		return ::bounded::assume_in_range<range_size_t<Target>>(value);
	}
};

} // namespace containers
