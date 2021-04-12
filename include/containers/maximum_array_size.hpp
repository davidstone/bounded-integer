// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/integer.hpp>

#include <cstddef>
#include <limits>

namespace containers {
namespace detail {

using namespace bounded::literal;

// TODO: support larger array sizes
template<typename T>
inline constexpr auto maximum_array_size = bounded::normalize<
	bounded::min(
		bounded::constant<bounded::max_value<std::ptrdiff_t>> / bounded::size_of<T>,
		(1_bi << 52_bi) - 1_bi
	).value()
>;

template<typename T>
using array_size_type = bounded::integer<0, maximum_array_size<T>>;

} // namespace detail
} // namespace containers
