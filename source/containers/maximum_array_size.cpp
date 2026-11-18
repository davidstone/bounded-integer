// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.maximum_array_size;

import bounded;
import numeric_traits;
import std_module;

namespace containers {
using namespace bounded::literal;

constexpr auto maximum_array_bytes = bounded::min(
	bounded::constant<numeric_traits::max_value<std::ptrdiff_t>>,
	(1_bi << 52_bi) - 1_bi
);

export template<typename T>
constexpr auto maximum_array_size = bounded::normalize<maximum_array_bytes / bounded::size_of<T>>;

export template<typename T>
using array_size_type = bounded::integer<0, maximum_array_size<T>>;

} // namespace containers