// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

#include <cstddef>

namespace containers::detail {

// TODO: Should this be limited to 2^52 - 1 bytes?
template<typename T>
inline constexpr auto maximum_array_size = bounded::normalize<bounded::max_value<std::ptrdiff_t> / sizeof(T)>;

template<typename T>
using array_size_type = bounded::integer<0, maximum_array_size<T>>;

} // namespace containers::detail
