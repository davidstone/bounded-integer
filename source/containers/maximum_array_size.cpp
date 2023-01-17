// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.maximum_array_size;

import bounded;
import numeric_traits;
import std_module;

namespace containers {

// TODO: Should this be limited to 2^52 - 1 bytes?
export template<typename T>
constexpr auto maximum_array_size = bounded::normalize<numeric_traits::max_value<std::ptrdiff_t> / sizeof(T)>;

export template<typename T>
using array_size_type = bounded::integer<0, maximum_array_size<T>>;

} // namespace containers