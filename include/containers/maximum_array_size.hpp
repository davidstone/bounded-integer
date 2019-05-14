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
constexpr auto maximum_array_size = bounded::detail::normalize<
	bounded::min(
		bounded::constant<std::numeric_limits<std::ptrdiff_t>::max()> / bounded::size_of<T>,
		(1_bi << 31_bi) - 1_bi
	).value()
>;

} // namespace detail
} // namespace containers
