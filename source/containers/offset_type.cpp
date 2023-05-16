// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.offset_type;

import containers.iter_difference_t;

import bounded;
import std_module;

namespace containers {

export template<typename T>
using to_offset_type = std::conditional_t<
	bounded::bounded_integer<T>,
	bounded::integer<0, bounded::builtin_max_value<T>>,
	T
>;

export template<typename T>
using offset_type = to_offset_type<iter_difference_t<T>>;

} // namespace containers

static_assert(std::same_as<containers::offset_type<int *>, std::ptrdiff_t>);
